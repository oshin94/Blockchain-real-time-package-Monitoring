from hashlib import sha256
from datetime import datetime

class Block():
  def __init__(self, tim, lat, lng, previous_block_hash):
    self.timestamp = datetime.utcnow()
    self.tim = tim
    self.lat = lat
    self.lng = lng
    self.prev_block_hash = previous_block_hash
    self.calculate_valid_hash()

  def is_hash_valid(self, hash):
        return (hash.startswith('0' * 3))

  def calculate_valid_hash(self):
      hash = ''
      nonce = 0

      while (not self.is_hash_valid(hash)):
          temp = self.to_string() + str(nonce)
          hash = sha256(temp.encode()).hexdigest()
          nonce += 1

      self.hash = hash

  def to_string(self):
    return "{0}\t{1}\t{2}\t{3}\t{4}".format(self.tim, self.lat, self.lng, self.timestamp, self.prev_block_hash)
  