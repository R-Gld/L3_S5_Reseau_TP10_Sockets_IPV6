## Using

To execute the server:
```bash
./build/server <port>
```

To execute the client (has to be executed after the server):

First, you have to find your ipv6 address from the `ip addr show` command.
Next use this:

```bash
./build/client <ipv6>%<interface> <port>
```

### Example

* `12888` is the port
* `fe80::1111:0000:1111:0000` is the ipv6 address
* `wlo1` is the interface

```bash
./build/server 12888 &
./build/client 'fe80::1111:0000:1111:0000%wlo1' 12888
```

