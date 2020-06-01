import requests
import json
from flask import Flask, request
import base64


app = Flask(__name__)
@app.route("/Spotify/Test")

def red():
    return "Hello, World"


clientID = '955b24586bc2412ca9b14fef5d29ccec'
clientSecret = 'a98ef98084654d0abf2b40a16ce16030'

endpoint = 'https://accounts.spotify.com/authorize'
redirect_uri = 'http://127.0.0.1:5000/Spotify/Test'
encoded_redirect = 'http%3A%2F%2F127.0.0.1%3A5000%2FSpotify%2FTest'

paramaters = {
    "client_id" : "{}".format(clientID),
    "response_type" : "code",
    "redirect_uri" : "{}".format(redirect_uri),
    "scope" : "user-modify-playback-state user-read-private playlist-modify-public user-library-modify user-top-read user-read-currently-playing playlist-read-private user-follow-read playlist-modify-private user-follow-modify user-library-read user-read-email"
}
response = requests.get(url=endpoint, params=paramaters)
print(response.url)
