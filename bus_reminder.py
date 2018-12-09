def do_connect():
    import network
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        wlan.connect("BaseHub-Devices", "Lorenco1995")
        while not wlan.isconnected():
            pass
    print('network config:', wlan.ifconfig())

def http_get(url):
    import socket
    _, _, host, path = url.split('/', 3)
    addr = socket.getaddrinfo(host, 80)[0][-1]
    s = socket.socket()
    s.connect(addr)
    s.send(bytes('GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n' % (path, host), 'utf8'))
    with open('data.txt', 'wb') as file:
        while True:
            data = s.recv(1024)
            if data:
                file.write(str(data, 'utf8'))
            else:
                break
        s.close()

def json_ify():
    with open('data.txt', 'rb') as file:
        with open('data.json', 'wb') as json_file:
            for line in file.readlines():
                if "data" in line:
                    json_file.write(line)

def parse_json():
    import ujson
    with open('data.json', 'rb') as json_file:
        ujson.loads(json_file.read())

def mem_check():
    import gc
    import micropython
    gc.collect()
    micropython.mem_info()
    print('-----------------------------')
    print('Initial free: {} allocated: {}'.format(gc.mem_free(), gc.mem_alloc()))

    def func():
        a = bytearray(10000)

    gc.collect()
    print('Func definition: {} allocated: {}'.format(gc.mem_free(), gc.mem_alloc()))
    func()
    print('Func run free: {} allocated: {}'.format(gc.mem_free(), gc.mem_alloc()))
    gc.collect()
    print('Garbage collect free: {} allocated: {}'.format(gc.mem_free(), gc.mem_alloc()))
    print('-----------------------------')
    micropython.mem_info(1)

def main():
    import gc
    http_get('http://nwoodthorpe.com/grt/V2/livetime.php?stop=2512')
    gc.collect()
    json_ify()
    gc.collect()
    parse_json()


main()