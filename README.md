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

## Server commands

### pong

Sent in reply to a client's 'ping' command.


```javascript
{
  "method": "pong",
  "server-time": "1476148417",
}
```

If a client does not receive a 'pong' for 45 seconds, it assumes connection has dropped and attempts reconnect.

### update

Used to modify a client's configuration.

For example: 

When a client first connects it has no ID, the server generates a UUID and uses the 'update' command to inform the client.

```javascript
{
  "method": "update",
  "key": "id",
  "value": "00000000-0000-0000-0000-000000000001"
}
```

### reboot

Reboots the client.

```javascript
{
  "method": "reboot"
}
