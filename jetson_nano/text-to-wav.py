def txt2wav(data, fileName):
    mapping = {
        "0": 0, "1": 1, "2": 2, "3": 3, "4": 4, "5": 5, "6": 6, "7": 7, "8": 8, "9": 9, "a": 10, "b": 11, "c": 12, "d": 13, "e": 14, "f": 15
    }

    with open(fileName+'.wav', 'wb') as file:
        check = False
        num = 0
        for i in data:
            if not(check):
                num = mapping[i]
                check = True
            else:
                num = num*16+mapping[i]
                file.write(bytes([num]))
                check = False
        file.close()

if __name__ == '__main__':
    filename = str(input())
    with open(filename, 'r') as file:
        data = file.read()

    txt2wav(data, filename)