import argparse
import json
import os
import spotipy
from datetime import datetime
from configparser import ConfigParser
from spotipy.oauth2 import SpotifyOAuth

class QMKSpot():
    def __init__(self, config_path, command):
        config = ConfigParser()
        config.read(config_path)

        self.client_id = config['AUTH']['client_id']
        self.client_secret = config['AUTH']['client_secret']
        self.username = config['AUTH']['username']
        self.scope = config['AUTH']['scope']
        self.redirect_uri = config['AUTH']['redirect_uri']

        self.device = config['CONFIG']['device']
        self.volume_delta = int(config['CONFIG']['volume_delta'])

        auth_manager = SpotifyOAuth(client_id=self.client_id,
                                    client_secret=self.client_secret,
                                    redirect_uri=self.redirect_uri,
                                    username=self.username,
                                    scope=self.scope)

        self.spotify = spotipy.Spotify(auth_manager=auth_manager)

        if not self.device:
            devices = self.spotify.devices()
            print("Set a device in config.ini:")
            for device in devices["devices"]:
                print(device["name"])

        for device in self.spotify.devices()['devices']:
            if device['name'] == self.device:
                self.device_id = device['id']
                break
        else:
            raise Exception("Device {} not found. Try opening the app on a device.".format(self.device))

        self.state = self.spotify.current_playback()

        self.command = command

    def handle_command(self):
        if self.command == "up":
            self.volume_up()
        elif self.command == "down":
            self.volume_down()
        elif self.command == "toggle":
            self.toggle_playback()
        elif self.command == "shuffle":
            self.toggle_shuffle()
        elif self.command == "next":
            self.next_track()
        elif self.command == "previous":
            self.previous_track()
        else:
            pass

    def toggle_playback(self):
        if self.state['is_playing']:
            self._pause()
        else:
            self._play()

    def toggle_shuffle(self):
        shuffle_state = self.state['shuffle_state']
        shuffle_state = not shuffle_state
        self.spotify.shuffle(state=shuffle_state, device_id=self.device_id)

    def volume_up(self):
        self._volume_change(self.volume_delta)

    def volume_down(self):
        self._volume_change(self.volume_delta*-1)

    def next_track(self):
        self.spotify.next_track(device_id=self.device_id)

    def previous_track(self):
        self.spotify.previous_track(device_id=self.device_id)

    def _volume_change(self, delta):
        volume_percent = self.state['device']['volume_percent']
        volume_percent += delta
        if volume_percent > 100:
            volume_percent = 100
        elif volume_percent < 0:
            volume_percent = 0
        self.spotify.volume(volume_percent)


    def _play(self):
        self.spotify.start_playback(device_id=self.device_id)

    def _pause(self):
        self.spotify.pause_playback(device_id=self.device_id)


def main():
    parser = argparse.ArgumentParser(description="Control Spotify")
    parser.add_argument(
        'command',
        action="store",
        type=str,
        choices=["up", "down", "toggle", "shuffle", "next", "previous"],
        help="command to control spotify"
    )

    arg = parser.parse_args()

    abspath = os.path.abspath(__file__)
    dirname = os.path.dirname(abspath)
    os.chdir(dirname)
    config_path = os.path.join(dirname, 'config.ini')
    me = QMKSpot(config_path=config_path, command=arg.command)
    me.handle_command()



if __name__ == '__main__':
    main()
