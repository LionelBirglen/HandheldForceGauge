import asyncio
import struct
import threading
import csv
import datetime

from bleak import BleakClient, BleakScanner
import matplotlib.pyplot as plt
import matplotlib.animation as animation

# ---------------------------------------------------------------------
# BLE identifiers (must match your Arduino sketch)
# ---------------------------------------------------------------------
SERVICE_UUID = "19B10010-E8F2-537E-4F6C-D104768A1214"
CHAR_UUID    = "19B10011-E8F2-537E-4F6C-D104768A1214"
DEVICE_NAME  = "Force Gauge"   # same as BLE.setLocalName() on the XIAO

# ---------------------------------------------------------------------
# Shared data buffer for plotting
# ---------------------------------------------------------------------
data = []
data_lock = threading.Lock()

# ---------------------------------------------------------------------
# CSV logging configuration
# ---------------------------------------------------------------------
CSV_FILENAME = "ForceGauge_Data.csv"
csv_lock = threading.Lock()

# Create / overwrite CSV file and write header once at start
with open(CSV_FILENAME, "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["timestamp", "value"])  # human-readable timestamp + value


def log_to_csv(value: float):
    """Append human-readable timestamp + value to the CSV file."""
    # Format: YYYY-MM-DD HH:MM:SS.mmm  (local time, millisecond precision)
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]

    with csv_lock:
        with open(CSV_FILENAME, "a", newline="") as f:
            writer = csv.writer(f)
            writer.writerow([timestamp, value])


def notification_handler(sender, value: bytearray):
    """
    Called in the BLE thread whenever a notification is received.
    'value' is a 4-byte float (little-endian) from the Arduino.
    """
    # Decode float
    f = struct.unpack('<f', value)[0]

    # Store for plotting
    with data_lock:
        data.append(f)

    # Log to CSV
    log_to_csv(f)

    print("Received:", f)


async def ble_loop():
    """Async BLE loop: scan, connect, and subscribe to notifications."""
    print("Scanning for BLE devices...")

    # Try finding by advertised name
    device = await BleakScanner.find_device_by_filter(
        lambda d, ad: d.name == DEVICE_NAME
    )

    # Fallback: try by service UUID if name not found
    if device is None:
        print("Not found by name. Trying by service UUID...")
        device = await BleakScanner.find_device_by_filter(
            lambda d, ad: SERVICE_UUID.lower() in [
                s.lower() for s in (ad.service_uuids or [])
            ]
        )

    if device is None:
        print("Device not found. Is the XIAO powered and advertising?")
        return

    print(f"Connecting to {device.address}...")
    async with BleakClient(device.address) as client:
        print("Connected!")

        # Start notifications on our characteristic
        await client.start_notify(CHAR_UUID, notification_handler)
        print("Notifications enabled — logging to CSV:", CSV_FILENAME)

        # Keep the BLE connection alive
        try:
            while True:
                await asyncio.sleep(1.0)
        except asyncio.CancelledError:
            pass
        finally:
            print("Stopping notifications…")
            await client.stop_notify(CHAR_UUID)
            print("BLE loop done.")


def start_ble_thread():
    """Start the BLE event loop in a background daemon thread."""
    def runner():
        asyncio.run(ble_loop())

    t = threading.Thread(target=runner, daemon=True)
    t.start()
    return t


def main():
    # Start BLE loop in background
    start_ble_thread()

    # Matplotlib figure in main thread
    fig, ax = plt.subplots()
    ax.set_title("Force Gauge Reading")
    ax.set_xlabel("Sample index")
    ax.set_ylabel("Force")

    line, = ax.plot([], [], lw=2)

    def update(frame):
        # Copy the last N samples for plotting
        with data_lock:
            if not data:
                return line
            window = data[-200:]  # last 200 samples

        line.set_data(range(len(window)), window)
        ax.relim()
        ax.autoscale_view()
        return line

    ani = animation.FuncAnimation(
        fig,
        update,
        interval=100,          # update plot every 100 ms
        cache_frame_data=False # avoids the warning you saw earlier
    )

    plt.show()


if __name__ == "__main__":
    main()
