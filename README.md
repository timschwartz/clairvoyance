# clairvoyance
Remote control/viewing software

# API
Commands are JSON encoded.

## Client commands

### authenticate
When a client connects to the server, it first sends an authentication command.


```javascript
{ 
  "method": "authenticate", 
  "shared-key": "shared secret" 
}
```

The server will ignore anything sent by the client before authentication occurs.

### ping
Sends a keep-alive packet to the server. 
Packet includes client's current time, clairvoyance version number, 
client's hostname and currently logged in user.

```javascript
{ 
  "method": "ping", 
  "id": "f40bbdcc-33aa-4c69-8eec-0b87c4776eb8",
  "client-time": "1476148417", 
  "client_version": "clairvoyance-client 0.1",
  "hostname": "computer-name", 
  "username": "user"
}
```
