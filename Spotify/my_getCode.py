import requests
import json
from flask import Flask, request
import base64
import urllib
import my_IDs 

app = Flask(__name__)
@app.route("/Spotify/Test")
def red():
    return request.url

# Importing my ID and Secret from file
clientID = my_IDs.clientID
clientSecret = my_IDs.clientSecret

endpoint = 'https://accounts.spotify.com/authorize'
redirect_uri = 'http://127.0.0.1:5000/Spotify/Test'
encoded_redirect = 'http%3A%2F%2F127.0.0.1%3A5000%2FSpotify%2FTest'

# Query paramaters
paramaters = {
    "client_id" : "{}".format(clientID),
    "response_type" : "code",
    "redirect_uri" : "{}".format(redirect_uri),
    "scope" : "user-modify-playback-state user-read-private playlist-modify-public user-library-modify user-top-read user-read-currently-playing playlist-read-private user-follow-read playlist-modify-private user-follow-modify user-library-read user-read-email"
}
# Get request
response = requests.get(url=endpoint, params=paramaters)
redirect = urllib.request.urlopen(response.url)

# Following this URL will allow user to give permission, and after that redirects the user to my webpage
# In the URL of that redirected webpage, there will be a "code" paramater. This code needs to be copied
# and used in the get request in the my_access_token module to get the final access token that will allow
# the main module to make requests and get data from the rest of Spotify's API

# I've tried everything possible to get the redirect uri back into my program and automate this process, but for
# some reason there apparently is no way to get the redirect uri outside the scope of the flask route function,
# (the function named "red"). The internet was no help either.

print(response.url)