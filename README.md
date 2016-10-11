# clairvoyance
Remote control/viewing software

# API
Commands are JSON encoded.

## Client commands

### authenticate
When a client connects to the server, it first sends an authentication command.


```
{ "method": "authenticate", "shared-key": "shared secret" }
```

The server will ignore anything sent by the server before authentication occurs.
