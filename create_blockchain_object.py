import pickle
from blockchain import Blockchain

Device = Blockchain()

inp = str(input("Enter the device ID: "))
with open(inp+".pkl", "wb") as f:
    pickle.dump(Device, f, pickle.HIGHEST_PROTOCOL)
print("Pickle object for Device id:", inp, ", created")

del Device