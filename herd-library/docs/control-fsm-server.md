# Minimal control application interface

This page summarises the 'network API' of the minimal skeleton of the control application, `control-fsm-server` - i.e. how to remotely access information about the devices, or run commands and FSM transitions.

`control-fsm-server` opens a ZMQ TCP socket on port 12345, and acts as the reply node in a request-reply pair of ZMQ sockets. In order to gather information and run commands and/or FSM transitions, another application should open a ZMQ connection to the `control-fsm-server` and send msgpack-encoded requests; upon receiving each request, `control-fsm-server` will attempt to carry out the requested actions, and then send back a msgpack-encoded reply. `control-app-console.py` is an example of a remote client for `control-fsm-server`; instructions on how to use `control-app-console.py` can be found in the README.

The sections below summarise the format of msgpack-encoded request messages that `control-fsm-server` supports, and the format of its response to each of these requests. Note: If the control app receives an invalid request, or if an exception is thrown when enacting any request, it will send the following response message:
```
["error", ERROR_MESSAGE]
```
where `ERROR_MESSAGE` is a string.


## Getting summary information about devices

Request:
```
["info-devices"]
```

Response:
```
["ok",
 {DEVICE_ID:
   {"role": DEVICE_ROLE,
    "type": DEVICE_CLASS,
    "uri": DEVICE_URI,
    "addressTable": DEVICE_ADDR_TABLE,
    "fsm": FSM_ID,
    "state" : STATE_ID
   }
 }
]
```


## Getting information about devices' registered commands

Request:
```
["info-commands"]
```

Response:
```
["ok",
 {DEVICE_ID:
   {COMMAND_ID:
     [COMMAND_ALIAS,
      {PARAMETER_ID: [PARAMETER_TYPE, RULE_DESCRIPTION]},
      {CONSTRAINT_NAME: [[PARAMETER_ID, ...], CONSTRAINT_DESCRIPTION]}
     ]
   }
 }
]
```


## Getting information about devices' registered FSMs

Request:
```
["info-fsms"]
```

Response:
```
["ok",
 {DEVICE_ID:
   {FSM_ID:
     [FSM_ALIAS,
      INITIAL_STATE,
      ERROR_STATE,
      {STATE_ID:
        {TRANSITION_ID:
          [TRANSITION_ALIAS, TRANSITION_END_STATE, [[COMMAND_ID, NAMESPACE], ...]]
        }
      }
     ]
   }
 }
]
```

## Running a command

Request:
```
["run-command"
 DEVICE_ID,
 COMMAND_ID,
 {PARAMETER_ID: PARAMETER_VALUE}
]
```

Response:
```
["ok",
 {"path": COMMAND_ID_PATH,
  "state": COMMAND_STATE,
  "duration": COMMAND_DURATION,
  "progress": COMMAND_PROGRESS,
  "message": COMMAND_LAST_MESSAGE
 }
]
```

## Engaging an FSM

Request:
```
["engage-fsm",
 DEVICE_ID,
 FSM_ID
]
```

Response:
```
["ok"]
```


## Resetting an FSM

Request:
```
["reset-fsm",
 DEVICE_ID,
 FSM_ID
]
```

Response:
```
["ok"]
```

## Disengaging an FSM

Request:
```
["disengage-fsm",
 DEVICE_ID,
 FSM_ID
]
```

Response:
```
["ok"]
```


## Running an FSM transition

Request:
```
["run-fsm-transition",
 DEVICE_ID,
 TRANSITION_ID,
 [{PARAMETER_ID: PARAMETER_VALUE}, ...]
]
```

Response:
```
["ok",
 {"path": TRANSITION_ID_PATH,
  "state": TRANSITION_STATE,
  "duration": TRANSITION_DURATION,
  "progress": TRANSITION_PROGRESS,
  "num-commands-done": TRANSITION_NUMBER_COMMANDS_DONE,
  "num-commands-total": TRANSITION_TOTAL_NUMBER_COMMANDS,
  "commands": 
    [{"path": COMMAND_ID_PATH,
      "state": COMMAND_STATE,
      "duration": COMMAND_DURATION,
      "progress": COMMAND_PROGRESS,
      "message": COMMAND_LAST_MESSAGE
     },
     ...
    ]
 }
]
```
