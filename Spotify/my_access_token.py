import requests
import base64
import datetime
import inspect

clientID = '955b24586bc2412ca9b14fef5d29ccec'
clientSecret = 'a98ef98084654d0abf2b40a16ce16030'
redirect_uri = 'http://127.0.0.1:5000/Spotify/Test'

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
    url_code = 'AQBp3ruVxy8HKPH6UcoqTqFtqX-5tFrx3BFf2oQRSvyuo-fpAby4uzXldKsBR2cyDfFxuiHk_M2zB3hvHhBEBuiSN_K8muGZzCKDSKbPnHnIInlR1vgIg71dtEXhZxZrS7Zkdxn4SPmCILG9XNx7a1d72kmVRPP_p6LcDgvLpDJUIFx9zbzMtNUT7gpPJuQfVVMAUAjmBEv50KoTdtvdYrMZB_ghssHD8Mpspnwd_dQepKws3aG6xc-IgcahfWcruPQeP2G3SiA_v_a8liJNFUCyzoKU6QxzbpBfcBJABEmKibMuFqkL9Ts8WK7cv5ITGsFG1zKGHOmtwPFYza1nP03QRV6eAXm2_cKxOu8qtM-apJmVG4wwGs1aabgI07J3hnSc4m3N0ttW8WEMapioZgtvQwcfVjLt4xbjpB_mVL5uFuFo_KxZWfUK54s7c8srmg9BEieqaZGs6G-LPZLGF5V95Dvb2YKFatU5ltfl8V-dgyVoOW_lU2PgJ48smF61FW46uUUGZnM'
    initial_data = initial_Auth(url_code)
    initial_json_data = initial_data.json()
    initial_access_token = initial_json_data["access_token"]
    initial_refresh_token = initial_json_data["refresh_token"]
    initial_expiration = datetime.datetime.now() + datetime.timedelta(initial_json_data["expires_in"])
    # Create bearer object with access token
    Bearer = User_info(initial_access_token, initial_refresh_token, initial_expiration)