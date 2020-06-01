# Artist class for data of an individual artist
class Artist:
    name = None
    followers = None
    popularity = None
    genres = []
    artist_id = None

    def __init__(self, name):
        self.name = name
