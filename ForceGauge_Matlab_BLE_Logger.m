function ForceGauge_Matlab_BLE_Logger
% TSA_SCALE_BLE_LOGGER
% Connects to the Force Gauge BLE device, receives float values from a
% characteristic, plots them live, and logs them to a CSV file with
% human-readable timestamps.
%
% Requirements:
%  - MATLAB with Bluetooth Low Energy support (`ble`, `characteristic`, etc.)
%  - XIAO nRF52840 advertising as "Force Gauge"
%  - BLE service/characteristic UUIDs must match the Arduino sketch

    % ---------------------------------------------------------------------
    % BLE identifiers (match your Arduino code)
    % ---------------------------------------------------------------------
    SERVICE_UUID = "19B10010-E8F2-537E-4F6C-D104768A1214";
    CHAR_UUID    = "19B10011-E8F2-537E-4F6C-D104768A1214";
    DEVICE_NAME  = "Force Gauge";   % BLE.setLocalName() in Arduino

    % ---------------------------------------------------------------------
    % CSV logging setup
    % ---------------------------------------------------------------------
    csvFileName = "ForceGauge_Data.csv";
    fileID = fopen(csvFileName, "w");
    if fileID == -1
        error("Could not open %s for writing.", csvFileName);
    end
    fprintf(fileID, "timestamp,value\n");

    % ---------------------------------------------------------------------
    % Connect to BLE device
    % ---------------------------------------------------------------------
    fprintf("Connecting to BLE device '%s'...\n", DEVICE_NAME);

    try
        % Simplest: connect by name (works if name is unique)
        dev = ble(DEVICE_NAME);
    catch
        % Fallback: scan and pick by name if direct connection fails
        fprintf("Direct connection failed. Scanning with blelist...\n");
        devList = blelist("Timeout", 5);
        idx = find(devList.Name == DEVICE_NAME, 1);
        if isempty(idx)
            fclose(fileID);
            error("Device '%s' not found. Is it advertising?", DEVICE_NAME);
        end
        address = devList.Address(idx);
        fprintf("Connecting to address %s...\n", address);
        dev = ble(address);
    end

    fprintf("Connected to %s (%s)\n", dev.Name, dev.Address);

    % Create characteristic object by UUID
    ch = characteristic(dev, SERVICE_UUID, CHAR_UUID);

    % ---------------------------------------------------------------------
    % Live plot setup
    % ---------------------------------------------------------------------
    values     = [];                 % numeric vector of readings
    timestamps = datetime.empty(0,1);% datetime vector (optional)

    fig = figure( ...
        "Name", "Force Gauge BLE Logger", ...
        "NumberTitle", "off", ...
        "CloseRequestFcn", @onFigureClose);

    hLine = plot(NaN, NaN, 'LineWidth', 2);
    grid on;
    xlabel("Sample index");
    ylabel("Force");
    title("Force Gauge Reading (BLE)");
    drawnow;

    % ---------------------------------------------------------------------
    % Setup BLE notification callback
    % ---------------------------------------------------------------------
    % When a new notification arrives, MATLAB will call displayCharacteristicData.
    ch.DataAvailableFcn = @displayCharacteristicData;

    % Subscribe to notifications (so DataAvailableFcn is triggered)
    subscribe(ch, "notification");
    fprintf("Subscribed to characteristic %s\n", CHAR_UUID);
    fprintf("Logging to %s\n", csvFileName);
    fprintf("Close the figure window to stop and clean up.\n");

    % Block here until the figure is closed
    waitfor(fig);  %#ok<*UNRCH>  % returns when figure is deleted

    % After figure closed, clean up in onFigureClose callback (below)


    % =====================================================================
    % Nested callback functions
    % =====================================================================

    function displayCharacteristicData(src, ~)
        % CALLBACK: called whenever a new BLE notification is available.
        % Uses read(src,'oldest') to fetch the oldest unread packet.
        % src is the characteristic object.

        try
            % Read "oldest" data and timestamp
            [raw, t] = read(src, 'oldest');   % raw is uint8, t is datetime

            if numel(raw) < 4
                return; % not enough bytes to decode float
            end

            % Decode 4-byte little-endian float (IEEE754) from Arduino
            raw4 = uint8(raw(1:4));
            v_single = typecast(raw4, 'single');
            v = double(v_single);  % promote to double for plotting

            % Append to data arrays
            values(end+1)     = v; 
            timestamps(end+1) = t;

            % Format timestamp as human-readable string
            t.Format = 'yyyy-MM-dd HH:mm:ss.SSS';
            tsStr = char(t);   % or string(t)

            % Log to CSV
            fprintf(fileID, '%s,%.6f\n', tsStr, v);

            % Update plot
            if isvalid(hLine)
                set(hLine, "XData", 1:numel(values), "YData", values);
                drawnow limitrate;
            end

            % Optional: print to command window
            fprintf("Received: %.6f at %s\n", v, tsStr);

        catch ME
            % Avoid crashing inside callback; just display error
            warning("Error in BLE callback: %s", ME.message);
        end
    end


    function onFigureClose(src, ~)
        % CALLBACK: executed when the figure window is closed.
        % Unsubscribe and clean up BLE + file.

        fprintf("Closing figure: cleaning up BLE and file...\n");

        % Unsubscribe if still connected
        try
            if isvalid(ch)
                unsubscribe(ch);
            end
        catch ME
            warning("Error during unsubscribe: %s", ME.message);
        end

        % Delete BLE object
        try
            if isvalid(dev)
                clear dev; 
            end
        catch
        end

        % Close CSV
        try
            fclose(fileID);
        catch
        end

        % Actually close the figure
        delete(src);
        fprintf("Cleanup done.\n");
    end

end
