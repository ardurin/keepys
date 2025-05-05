from base64 import b64decode, b64encode
from cryptography.hazmat.primitives.ciphers.aead import ChaCha20Poly1305
from random import randbytes, shuffle
from sys import argv
import string


def create():
	password = input('Password: ')
	password = password.ljust(32, '\0')
	password = password.encode()[:32]

	nonce = randbytes(12)

	secret = list(string.printable[:94].encode())
	shuffle(secret)
	secret = bytes(secret[:16])
	print(secret.decode())

	cipher = ChaCha20Poly1305(password)
	data = cipher.encrypt(nonce, secret, None)
	secret = data[:16]
	authentication = data[16:]

	with open(argv[2], 'wb') as stream:
		stream.write(b64encode(bytes([*authentication, *nonce, *secret])))


def get():
	with open(argv[2], 'rb') as stream:
		data = b64decode(stream.read())
		authentication = data[:16]
		nonce = data[16:28]
		secret = data[28:]

		password = input('Password: ')
		password = password.ljust(32, '\0')
		password = password.encode()[:32]

		cipher = ChaCha20Poly1305(password)
		print(cipher.decrypt(nonce, bytes([*secret, *authentication]), None).decode())


if len(argv) == 3:
	if argv[1] == 'get':
		get()
		exit()
	elif argv[1] == 'create':
		create()
		exit()
print('Usage: test.py {create|get} <file>')
