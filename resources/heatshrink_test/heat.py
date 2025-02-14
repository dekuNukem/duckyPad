import heatshrink2

input_file = open("key1.txt", 'rb')
content = input_file.read()
input_file.close()

print(len(content))

result = heatshrink2.compress(content)

print(len(result))

decompd = heatshrink2.decompress(result)

print(decompd)