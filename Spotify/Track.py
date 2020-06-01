# Track class to keep data for a single song
class Track:
    name = None
    artist = None
    is_explicit = None
    track_id = None
    popularity = None
    duration = None

    def __init__(self, name):
        self.name = name