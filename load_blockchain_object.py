from blockchain import Blockchain
import pickle

while True:
    dev = str(input("Enter name of Device id: "))

    # with open(inp+".pkl", "rb") as f:
        # blockchain = pickle.load(f)
    # for block in blockchain.get_blocks():
        # print()
        # print('\t' + block.to_string())

    while True:
        inp = str(input("\na: Add data in device "+dev+"\nv: View Data in device "+dev+"\nn: Enter new device name\ne: Exit\nGet Input: "))
        if inp=='a':
            try:
                with open("./pickle/"+dev+".pkl", "rb") as f:
                    blockchain = pickle.load(f)
                x = str(input("Enter data to store: "))
                blockchain.add_new_block(x);
                with open("./pickle/"+dev+".pkl", "wb") as f:
                    pickle.dump(blockchain, f, pickle.HIGHEST_PROTOCOL)
                del blockchain
            except:
                print("Device:,",dev,"doesn't exist")
                break
        elif inp=='v':
            try:
                with open("./pickle/"+dev+".pkl", "rb") as f:
                    blockchain = pickle.load(f)
            except:
                print("Device:,",dev,"doesn't exist")
                break
            for block in blockchain.get_blocks():
                print()
                print('\t' + block.to_string())
            del blockchain
            
        elif inp=='n':
            break
        elif inp=='e':
            break;
    if inp=='e':
        break;
print("Exited from program")
