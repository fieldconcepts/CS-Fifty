import sys

if len(sys.argv) == 2:

    k = int(sys.argv[1])
    
    print("Key: {}".format(k))

    print("plaintext: ", end="")
    plaintext = str(input())
    
    print("ciphertext: ")
    
    
    for i in range(0, len(plaintext), 1):
        
        if (plaintext[i].isalpha()):
            
            if (plaintext[i].isupper()):

                charval = chr(((ord(plaintext[i]) - 65 + k) % 26) + 65)
                
                print(charval, end="")
            
            elif (plaintext[i].islower()):
                
                charval = chr(((ord(plaintext[i]) - 97 + k) % 26) + 97)
                print(charval, end="")

        else:
            print(plaintext[i], end="")
    
    print("\n")
    
else:
    print("invalid Command")







