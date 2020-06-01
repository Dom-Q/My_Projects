import requests
import json
import pandas
from statistics import mode
from sklearn.preprocessing import StandardScaler, RobustScaler
from sklearn.model_selection import train_test_split
from sklearn import neighbors, preprocessing, tree
from sklearn.tree import DecisionTreeClassifier
import numpy

class Seed:
    num_genres = 0
    num_songs = 0
    num_artists = 0
    seed_list = []

    def __init__(self):
        pass

class Spotify:

    #==================#
    # Member variables #
    #==================#
#---------------------------#
    access_token = None
    user_id = None
    user_data = None
    playlist_data = None
    playlist_ID = None
    #Json artist data
    top_artists_data = None
    #Json track data
    top_tracks_data = None
    #List of artist objects
    top_artists = []
    #List of track objects
    top_tracks = []
    #List of lists containing audio features for each song
    audio_features = []
    num_top_tracks = None
    ML_label = ["danceability", "energy", "key", "loudness", "mode", "speechiness", "acousticness", "instrumentalness", 
            "liveness", "valence", "tempo"]
    Dataset = None
    # Recommended Json
    recommended_song_data = []
    # List of recommended Spotify track objects
    recommended_songs = []
    # List of sppotify ids of recommended songs by spotify
    recommended_ids = []
    # List of 1s and 0s to represent user opinion on their top songs (personalization data)
    # New playlist songs to be added
    final_playlist_song_names = []
    user_ratings = []
    # List of 1s and 0s based on ML algorithm based on whether or not user will like Spotify recommended songs
    ML_predictions = []
    # Spotify recommended songs that Tree decided will be good for user
    hit_uris = []
    # Hit obj to handle multiple returns
    class Hit_data:
        indices = []
        valid_recs = []

        def __init__(self):
            pass
    Hit_object = None

#----------------------------#
        #=========#
        # Methods #
        #=========#
#----------------------------#

    # Constructor
    # Sets access token
    def __init__(self, token):
        self.access_token = token

    # Returns User data
    def get_user_data(self, token):
        user_endpoint = 'https://api.spotify.com/v1/me'
        header = {
            "Authorization":"Bearer {}".format(token)
        }
        response = requests.get(url=user_endpoint, headers=header)
        print("User Data Request Status = {}".format(response))
        return response.json()

    # Returns data of User's top artists 
    def artists_personalization_data(self, token):
        url = "https://api.spotify.com/v1/me/top/artists"
        header = {
            "Authorization" : "Bearer {}".format(token)
        }
        paramater = {
            "limit" : 7
        }
        resp = requests.get(url=url, headers=header, params=paramater)
        print("Personalization Response Data Status = {}".format(resp))
        return resp.json()

    # Returns data of User's top tracks 
    def tracks_personalization_data(self, token):
        url = "https://api.spotify.com/v1/me/top/tracks"
        header = {
            "Authorization" : "Bearer {}".format(token)
        }
        paramater = {
            "limit" : 50
        }
        resp = requests.get(url=url, headers=header, params=paramater)
        print("Personalization Response Data Status = {}".format(resp))
        personalization = resp.json()
        return personalization

    # Creates Playlist within User Account and Returns its Data 
    def create_Playlist(self, token, user_id):
        playlist_endpoint = "https://api.spotify.com/v1/users/{}/playlists".format(user_id)
        #header data
        playlist_header = {
            "Authorization" : "Bearer {}".format(token),
            "Content-Type" : "application/json"
        }
        #request body
        playlist_body = json.dumps({
            "name" : "New Song Recommendations",
            "description" : "Based on your listening history, you may like some of these songs"
        })
        response = requests.post(url=playlist_endpoint, headers=playlist_header, data=playlist_body)
        print("Playlist Creation Response Status = {}".format(response))
        return response.json()
    
    # Print user's top artis features
    def print_artists_features(self):
        for artist in self.top_artists:
            print("{} - {} - {} - {}".format(artist.name, artist.followers, artist.popularity, artist.genres))
    
    # Print user's top track features
    def print_tracks_features(self):
        for track in self.top_tracks:
            print("{} - {} - {} - {} - {}".format(track.name, track.popularity, track.artist, track.track_id, track.duration))

    # Get audio features of user top tracks
    # Returns list of each song's audio features
    def get_audio_features(self):
        track_ids = []
        for track in self.top_tracks:
            track_ids.append(track.track_id)
        header = {
            "Authorization" : "Bearer {}".format(self.access_token)
        }
        return_list = []
        for the_id in track_ids:
            endpoint = "https://api.spotify.com/v1/audio-features/{}".format(the_id)
            feature_data = requests.get(url=endpoint, headers=header)
            json_data = feature_data.json()
            return_list.append(json_data)
        print("Audio Features Response Data Status = {}".format(feature_data))
        return return_list

    def count_top_tracks(self):
        return len(self.top_tracks)


    # Seeding
    def make_seeds(self):
        seed_obj = Seed()
        #Create list of all genres
        all_genres = []
        for each in self.top_artists:
            for genre in each.genres:
                all_genres.append(genre)
        length = len(all_genres) - 1
        # Append 1st top genre
        seed_obj.seed_list.append(mode(all_genres))
        seed_obj.num_genres = seed_obj.num_genres + 1
        genre_1 = str(mode(all_genres))
        while length >= 0:
            if all_genres[length] == genre_1:
                    all_genres.remove(all_genres[length])
            length = length - 1
        #Append 2nd top genre
        if all_genres:
            (seed_obj.seed_list).append(mode(all_genres))
            seed_obj.num_genres = seed_obj.num_genres + 1
        #Append top 2 artists
        (seed_obj.seed_list).append(self.top_artists[0].artist_id)
        seed_obj.num_artists = seed_obj.num_artists + 1
        if len(self.top_artists) > 1:
            seed_obj.seed_list.append(self.top_artists[1].artist_id)
            seed_obj.num_artists = seed_obj.num_artists + 1
        #Append top song
        seed_obj.seed_list.append(self.top_tracks[0].track_id)
        while len(seed_obj.seed_list) < 5:
            seed_obj.seed_list.append(self.top_tracks[1].track_id)
            seed_obj.num_songs = seed_obj.num_songs + 1
        return seed_obj

    def get_recommendations(self):
        endpoint = 'https://api.spotify.com/v1/recommendations'
        header = {
            "Authorization" : "Bearer {}".format(self.access_token)
        }
        seed_obj = self.make_seeds()
        genre_seed = []
        song_seed = []
        artist_seed = []
        i = 0
        while i < seed_obj.num_genres:
            genre_seed.append(seed_obj.seed_list[i])
            i = i + 1
        k = i
        while i < (seed_obj.num_artists + k):
            artist_seed.append(seed_obj.seed_list[i])
            i = i + 1
        while i < 5: 
            song_seed.append(seed_obj.seed_list[i])
            i = i + 1
        q = {
            "limit" : 100,
            "seed_genres" : genre_seed,
            "seed_artists" : artist_seed,
            "seed_tracks" : song_seed
        }
        response = requests.get(url=endpoint, headers=header, params=q)
        data = response.json()
        print("Recommendations Response Status = {}".format(response))
        return data

    # Get ML data
    def get_ML_data(self):
        ML_dataset = []
        for song in self.audio_features:
            ML_dataset.append(tuple([song[0], song[1], song[2], -1*song[3], song[4], song[5], song[6], song[7],
                                    song[8], song[9], song[10]]))
        return pandas.DataFrame(data=ML_dataset, columns=self.ML_label)
    
    # Prompt user for input to fill Spotify.user_ratings
    def get_song_ratings(self):
        print("In order for us to recommend songs that you hopefully enjoy, we need your current opinion on each of your top songs, as determined by Spotify.")
        print("For each songs that comes up, we need you to vote whether its one of your favorites of your top songs. If it's not the best of the best, enter 0 for that song. If it is, enter 1.")

        self.num_top_tracks = self.count_top_tracks()
        print("You have : {} top songs".format(self.num_top_tracks))
        song_ratings = []
        for trk in self.top_tracks:
            print("Song: {} by {}".format(trk.name, trk.artist))
            answer = 2
            answer = input("Your Rating (1-Best - 0 Not best) : ")
            while(not((answer == "0") or (answer == "1"))):
                print("You need to enter an answer for this song: 1 for a favorite and 0 if not")
                answer = input("Your Rating (1-Best - 0 Not best) : ")
            song_ratings.append(answer)
        return song_ratings
    
    # Decision Tree algorithm to determine whether or not user will like recommended songs
    # Returns list of predictions (1 or 0)
    # Param = label - list of opinions of user current top songs
    def Decision_tree_predictions(self):
        # Get ML Dataset
        self.Dataset = self.get_ML_data()
        my_list = ['1','1','1','1','0','0','0','1','0','1','1','0','1','1','0','1','1','0','0','1','1','1','1','0','1','1','0','0','1','0','0','1','0','0','1','0','0','0','0','0','0','0','1','1','0','1','0','0','0','1']
        self.Dataset["Ratings"] = my_list

        #self.Dataset["Ratings"] = self.user_ratings
        
        # Organize features and labels
        drop_labels = ["Ratings"]
        X = numpy.array((self.Dataset).drop(drop_labels, axis=1))
        y = numpy.array(self.Dataset["Ratings"])
        X_test = X
        X_train = X
        y_train = y
        y_test = y
        # Decision Tree
        Decision_Tree = DecisionTreeClassifier(min_samples_split=10)
        Decision_Tree.fit(X_train, y_train)
        accuracy = Decision_Tree.score(X_test,y_test)
        print(accuracy)
        tester = (numpy.array(self.audio_features))
        prediction = Decision_Tree.predict(tester)
        return prediction
    
    def get_hits(self, list_of_hits):
        self.Hit_object = self.Hit_data()
        i = 0
        self.Hit_object.indices = []
        self.Hit_object.valid_recs = []
        for rating in list_of_hits:
            if rating == '1' and ((len(self.hit_uris) + len(self.Hit_object.valid_recs) + 1) <= 50):
                self.Hit_object.valid_recs.append(self.recommended_songs[i]["uri"])
                self.Hit_object.indices.append(i)
                i = i + 1
            else:
                i = i + 1
        return

    def add_songs(self, playlist_ID, uri_list):
        url = 'https://api.spotify.com/v1/playlists/{}/tracks'.format(playlist_ID)
        header = {
            "Authorization" : "Bearer {}".format(self.access_token),
            "Content-Type" : "application/json"
        }
        paramaters = json.dumps({
            "uris" : uri_list
        })
        response = requests.post(url=url, headers=header, data=paramaters)
        print("Add_Songs Status Report = {}".format(response))

