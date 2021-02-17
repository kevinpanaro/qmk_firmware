import hid
import os
from configparser import ConfigParser



class QMKDevice():
    def __init__(self, config_path):
        config = ConfigParser()
        config.read(config_path)

        def _to_int(string, base=16):
            return int(string, base)

        self.vid = _to_int(config['DEVICE']['VID'])
        self.pid = _to_int(config['DEVICE']['PID'])
        self.usage_page = _to_int(config['DEVICE']['USAGE_PAGE'])
        self.usage = _to_int(config['DEVICE']['USAGE'])

        self.payload = None

        devices = hid.enumerate()
        for device in devices:
            if (
                device["vendor_id"] == self.vid
                and device["product_id"] == self.pid
                and device["usage_page"] == self.usage_page
                and device["usage"] == self.usage
            ):
                self.device = hid.Device(path=device['path'])
                break

    def get_device_info(self):
        print("Device manufacturer: {}".format(self.device.manufacturer))
        print("Product: {}".format(self.device.product))

    def send(self, data, line):
        data = self.tobytes([line, data])
        self.device.write(self.pad(data))

    def close(self):
        self.device.close()

    def pad(self, data):
        return data + b'\x00' * (64 - len(data))

    def tobyte(self, data):
        if type(data) is bytes:
            return data
        else:
            return (data).to_bytes(1, 'big')

    def tobytes(self, data):
        out = b''
        for num in data:
            out += self.tobyte(num)
        return out

    def clear_screen(self):
        self.send(b'', 8)

def main():
    abspath = os.path.abspath(__file__)
    dirname = os.path.dirname(abspath)
    os.chdir(dirname)
    config_path = os.path.join(dirname, 'config.ini')

    try:
        me = QMKDevice(config_path)
        me.send(data=b'It works!', line=3)
    finally:
        me.close()

if __name__ == '__main__':
    main()
