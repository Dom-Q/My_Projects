import my_access_token
from Spotify import Spotify
from Artist import Artist
from Track import Track
import math
from sklearn.metrics import accuracy_score
from my_access_token import Bearer
import sys

access_token = Bearer.fin_token
Account_obj = Spotify(access_token)

# Get user profile data
user_data = Account_obj.get_user_data(Account_obj.access_token)
Account_obj.user_id = user_data["id"]
Account_obj.user_data = user_data

# Get user top artist data
Account_obj.top_artists_data = Account_obj.artists_personalization_data(Account_obj.access_token)

# Get user top track data
Account_obj.top_tracks_data = Account_obj.tracks_personalization_data(Account_obj.access_token)

# Create new playlist
new_playlist_data = Account_obj.create_Playlist(Account_obj.access_token, Account_obj.user_id)
Account_obj.playlist_data = new_playlist_data
Account_obj.playlist_ID = Account_obj.playlist_data['id']

# Create list of User's top artists
list_of_artists = []
for item in Account_obj.top_artists_data["items"]:
    list_of_artists.append(Artist(item["name"]))

# Instantiate rest of top artist data
i = 0
for artist in list_of_artists:
    temp = []
    list_of_artists[i].followers = Account_obj.top_artists_data["items"][i]["followers"]["total"]
    for this_genre in Account_obj.top_artists_data["items"][i]["genres"]:
        if(this_genre == 'hip hop' or this_genre == 'country' or this_genre == 'jazz' or this_genre =='classical'
           or this_genre == 'heavy metal' or this_genre == 'folk' or this_genre == 'rock' or this_genre == 'hard-rock'
           or this_genre == 'blues' or this_genre == 'r-n-b' or this_genre == 'reggae'
           or this_genre == 'soul' or this_genre == 'techno' or this_genre == 'punk' or this_genre == 'edm'
           or this_genre == 'electronic' or this_genre == 'gospel' or this_genre == 'dubstep' or this_genre == 'alternative'
           or this_genre == 'classical'):   
            temp.append(this_genre)
    list_of_artists[i].genres = temp
    list_of_artists[i].popularity = Account_obj.top_artists_data["items"][i]["popularity"]
    list_of_artists[i].artist_id = Account_obj.top_artists_data["items"][i]["id"]
    i = i+1

Account_obj.top_artists = list_of_artists

# Create list of User's top tracks
list_of_tracks = []
for song in Account_obj.top_tracks_data["items"]:
    list_of_tracks.append(Track(song["name"]))

# Instantiate rest of top track 
i = 0
for song in list_of_tracks:
    list_of_tracks[i].track_id = Account_obj.top_tracks_data["items"][i]["id"]
    list_of_tracks[i].is_explicit = Account_obj.top_tracks_data["items"][i]["explicit"]
    list_of_tracks[i].popularity = Account_obj.top_tracks_data["items"][i]["popularity"]
    list_of_tracks[i].duration = ((Account_obj.top_tracks_data["items"][i]["duration_ms"]) / 1000) / 60
    list_of_tracks[i].artist = Account_obj.top_tracks_data["items"][i]["artists"][0]['name']
    i = i + 1

Account_obj.top_tracks = list_of_tracks

for one in Account_obj.top_tracks:
    Account_obj.top_track_names.append(one.name)

# Instantiate total number of top tracks
Account_obj.num_top_tracks = Account_obj.count_top_tracks()
if Account_obj.num_top_tracks < 25:
    print("You need at least 25 songs for this algorithm to be reliable. Listen to some more songs on spotify and try again tomorrow!")
    sys.exit()
# Get list of audio audio features 
audio_features = []
audio_features = Account_obj.get_audio_features()
Account_obj.audio_features = audio_features
#Convert each dictionary to a list
h = 0
for each in Account_obj.audio_features:
    audio_features[h] = [Account_obj.audio_features[h]["danceability"], Account_obj.audio_features[h]["energy"], 
                        Account_obj.audio_features[h]["key"], Account_obj.audio_features[h]["loudness"],
                        Account_obj.audio_features[h]["mode"], Account_obj.audio_features[h]["speechiness"], 
                        Account_obj.audio_features[h]["acousticness"], Account_obj.audio_features[h]["instrumentalness"], 
                        Account_obj.audio_features[h]["liveness"], Account_obj.audio_features[h]["valence"], 
                        Account_obj.audio_features[h]["tempo"]]
    h = h + 1


# Get song ratings from user
Account_obj.user_ratings = Account_obj.get_song_ratings()

# ML and track searching
tracklist_size = 0
# Loops until 50 songs are in the playlist
while(tracklist_size < 50):
    Account_obj.recommended_song_data = []
    Account_obj.recommended_songs = []
    Account_obj.recommended_ids = []
    # Get song recommendations and song id list
    Account_obj.recommended_song_data = Account_obj.get_recommendations()
    Account_obj.recommended_songs = Account_obj.recommended_song_data["tracks"]
    for song in Account_obj.recommended_songs:
        Account_obj.recommended_ids.append(song["id"])

    # Decision Tree Prediction
    Account_obj.ML_predictions = Account_obj.Decision_tree_predictions()

    # Get hits
    Account_obj.get_hits(Account_obj.ML_predictions)
    # Append uris to Account object
    for item in Account_obj.Hit_object.valid_recs:
        Account_obj.hit_uris.append(item)
    tracklist_size = len(Account_obj.hit_uris)
    # Get song names using indices retrieved from ML algorithm
    for index in Account_obj.Hit_object.indices:
        Account_obj.final_playlist_song_names.append(Account_obj.recommended_songs[index]["name"])

# How many songs are in the list to be added to playlist. Should be 50 once complete.   
print(len(Account_obj.hit_uris))

# Add Songs to playlist
Account_obj.add_songs(Account_obj.playlist_ID, Account_obj.hit_uris)








