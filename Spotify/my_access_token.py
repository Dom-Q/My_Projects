import requests
import base64
import datetime
import inspect
import my_IDs

# This file retrieves the access token needed to make requests to the Spotify API.
# DO NOT RUN THIS FILE. Only paste the code from the redirect uri into the "URL_code" variable

# Substitute these with your own ID and Secret
clientID = my_IDs.clientID
clientSecret = my_IDs.clientSecret
redirect_uri = 'http://127.0.0.1:5000/Spotify/Test'

# Supposed to refresh my code once it expires using the refresh token but doesn't work for some reason.
# The access token you receive will have to be manually refreshed after 1 hour
def perform_Auth(refresh):
    next_endpoint = 'https://accounts.spotify.com/api/token'
    body = {
        "grant_type" : "authorization_code",
        "code" : "{}".format(refresh),
        "redirect_uri" : "{}".format(redirect_uri)
    }

    credentials = f"{clientID}:{clientSecret}"
    credentials_64 = base64.b64encode(credentials.encode())
    header = {
        "Authorization" : f"Basic {credentials_64.decode()}"
    }

    return requests.post(url=next_endpoint, headers=header, data=body)

# Post request which returns json data including the access and refresh token
def initial_Auth(url_code):
    next_endpoint = 'https://accounts.spotify.com/api/token'
    body = {
        "grant_type" : "authorization_code",
        "code" : "{}".format(url_code),
        "redirect_uri" : "{}".format(redirect_uri)
    }

    credentials = f"{clientID}:{clientSecret}"
    credentials_64 = base64.b64encode(credentials.encode())
    header = {
        "Authorization" : f"Basic {credentials_64.decode()}"
    }

    return requests.post(url=next_endpoint, headers=header, data=body)

class User_info:
    fin_token = None
    refresher = None
    expires = datetime.datetime.now()
    num_instances = 0
    def __init__(self, token, token1, expires_in):
        self.fin_token = token
        self.refresher = token1
        self.expires = expires_in
        self.num_instances = self.num_instances+1

    def check_expiration(self):
        now = datetime.datetime.now()
        if(now < self.expires):
            return
        else:
            response = perform_Auth(self.refresher)
            json_data = response.json()
            
            expires_in = json_data["expires_in"]
            self.expires = datetime.datetime.now() + datetime.timedelta(seconds=expires_in)
            
            self.fin_token = json_data["access_token"]
            self.refresher = json_data["refresh_token"]
            return

# Uses code in url to obtain access token and refresh token from api
if(inspect.isclass(User_info) == True):
    # Enter the code retrieved from redirect uri here
    url_code = 'AQBkorWCl3bv71nC9G1LddL9ei-FaEtYAQWKlFTwaARBC4vH4NKbmRZZMSg11m5TS_mGcIzCU652J01nVaKfQiU0-zlKUZOBcFbBeaO6pXHD4mbb05qvcBu8o4kCujNobAKe1aGb1xDhE7_BFDa1hXpahsNVgzqwSVEifcMYvID7HMXXUh0-ROWhJNN06DJwJLCRxCdGDjZXPa2c0E4J2CqOirkoGhZAGFMt2kbeEwuFSRdYKvJYlL688xd-voEpkLgMyK5aMYZwnjVGCWV5Ow36dIt29lKX-WSDH_KuF7MoApUWo8vK4aYPikwsTNpl0_9WwgdY4Heo6i0tG6YM5q-pER27NfdmA56HlsEtX3D2Rtrii11o3IQxVXX1fePLR04pZsVL9K_-DGp2vcUH9JjGg7JmhtxwmVnA6sH0fvyhUsys8jdo8srQ6QuAyCrSrTxNmfU1EHqQgFF0uPKteWjQ_9nNzWLCeZPOX8Bizyg1AGehwGXKtHuYzKsyYYvp4kJg9oBSEcE'
    initial_data = initial_Auth(url_code)
    initial_json_data = initial_data.json()
    initial_access_token = initial_json_data["access_token"]
    initial_refresh_token = initial_json_data["refresh_token"]
    initial_expiration = datetime.datetime.now() + datetime.timedelta(initial_json_data["expires_in"])
    # Create bearer object with access token
    Bearer = User_info(initial_access_token, initial_refresh_token, initial_expiration)