#!/bin/env python

'''
control-app-console.py: A simple console used for initial testing 
    of HERD plugins with minimal prototype control application

Available commands:
 * info                                     - Print summary of registered devices, commands and FSMs
 * run-command DEVICE_ID COMMAND_ID         - Run specified command
 * engage-fsm DEVICE_ID FSM_ID              - Engage specified FSM
 * reset-fsm DEVICE_ID FSM_ID               - Reset specified FSM to initial state
 * disengage-fsm DEVICE_ID FSM_ID           - Disengage specified FSM
 * run-transition DEVICE_ID TRANSITION_ID   - Run specified FSM transition
 * exit                                     - Close the console
'''

from __future__ import print_function
from __future__ import unicode_literals

import msgpack
import shlex
import sys
import zmq

from collections import namedtuple
from prompt_toolkit import PromptSession
from prompt_toolkit.completion import Completer, Completion, WordCompleter
from prompt_toolkit.document import Document
from prompt_toolkit.validation import Validator, ValidationError




# PART 1: STRUCTS REPRESENTING DATA RECEIVED FROM CONTROL APP

Board = namedtuple('Board', ['devices'])
Device = namedtuple('Device', ['id', 'uri', 'addressTable', 'type', 'commands', 'fsms'])

Command = namedtuple('Command', ['id', 'alias', 'parameters', 'constraints'])
Parameter = namedtuple('Parameter', ['id', 'type', 'rule'])
Constraint = namedtuple('Constraint', ['name', 'parameters', 'description'])

FSM = namedtuple('FSM', ['id', 'alias', 'initialState', 'errorState', 'states'])
State = namedtuple('State', ['id', 'transitions'])
Transition = namedtuple('Transition', ['id', 'alias', 'end', 'commands'])

CommandSnapshot = namedtuple('CommandSnapshot', ['path', 'state', 'duration', 'progress', 'message'])



# PART 2: FUNCTIONS FOR INTERACTING WITH CONTROL APP

ZMQ_CONTEXT = zmq.Context()


def connect(hostname, port):
    socket = ZMQ_CONTEXT.socket(zmq.REQ)
    socket.connect("tcp://{}:{}".format(hostname, port))
    return socket


def getInfo(socket):
    socket.send(msgpack.packb(['info-devices']))
    rawDeviceInfo = msgpack.unpackb(socket.recv())

    socket.send(msgpack.packb(['info-commands']))
    rawCommandInfo = msgpack.unpackb(socket.recv())

    socket.send(msgpack.packb(['info-fsms']))
    rawFsmInfo = msgpack.unpackb(socket.recv())

    board = Board(devices=[])
    for deviceId in rawDeviceInfo[1]:
        uri = rawDeviceInfo[1][deviceId]['uri']
        addrTable = rawDeviceInfo[1][deviceId]['addressTable']
        deviceType = rawDeviceInfo[1][deviceId]['type']

        device = Device(id=deviceId, uri=uri, addressTable=addrTable, type=deviceType, commands=[], fsms=[])

        for commandId, (alias, paramMap, constraintMap) in rawCommandInfo[1][deviceId].iteritems():
            c = Command(id=commandId, alias=alias, parameters=[], constraints=[])
            for paramId, (paramType, paramRule) in paramMap.iteritems():
                c.parameters.append(Parameter(paramId, paramType, paramRule))
            for constraintName, (parameterIds, description) in constraintMap.iteritems():
                c.constraints.append(Constraint(constraintName, parameterIds, description))
            device.commands.append(c)

        for fsmId, (fsmAlias, intialState, errorState, stateMap) in rawFsmInfo[1][deviceId].iteritems():
            fsm = FSM(fsmId, fsmAlias, intialState, errorState, [])
            for stateId, transitionMap in stateMap.iteritems():
                s = State(stateId, [])
                for transitionId, (transitionAlias, endState, commands) in transitionMap.iteritems():
                    s.transitions.append(Transition(id=transitionId, alias=transitionAlias, end=endState, commands=commands))
                fsm.states.append(s)
            device.fsms.append(fsm)

        board.devices.append(device)

    return board


def runCommand(socket, deviceId, commandId, parameters):

    print("Executing the command ...")
    socket.send(msgpack.packb(['run-command', deviceId, commandId, parameters]))
    reply = msgpack.unpackb(socket.recv())

    if reply[0] == 'error':
        raise RuntimeError("Control app reported an error when running command '{}' (device ID: {}): {}".format(commandId, deviceId, reply[1]))

    if reply[1]["state"] in ("Done", "Warning", "Error"):
        if reply[1]["state"] == "Done":
            print("  Command completed successfully!")
        elif reply[1]["state"] == "Warning":
            print("  Command completed with warning!")
        else:
            print("  Error occurred in command!")
        print("  Took {} seconds".format(reply[1]['duration']))
        print("  Progress: {}%".format(reply[1]['progress'] * 100, ))
        print("  Last status message:", reply[1]['message'])
    else:
        print('ERROR: Unexpected command state: "{}"'.format(reply[1]["state"]))



def engageFSM(socket, deviceId, fsmId):
    socket.send(msgpack.packb(['engage-fsm', deviceId, fsmId]))
    reply = msgpack.unpackb(socket.recv())

    if reply[0] == 'error':
        raise RuntimeError("Control app reported an error when engaging FSM '{}' (device ID: {}): {}".format(fsmId, deviceId, reply[1]))


def resetFSM(socket, deviceId, fsmId):
    socket.send(msgpack.packb(['reset-fsm', deviceId, fsmId]))
    reply = msgpack.unpackb(socket.recv())

    if reply[0] == 'error':
        raise RuntimeError("Control app reported an error when resetting FSM '{}' (device ID: {}): {}".format(fsmId, deviceId, reply[1]))


def disengageFSM(socket, deviceId, fsmId):
    socket.send(msgpack.packb(['disengage-fsm', deviceId, fsmId]))
    reply = msgpack.unpackb(socket.recv())

    if reply[0] == 'error':
        raise RuntimeError("Control app reported an error when disengaging FSM '{}' (device ID: {}): {}".format(fsmId, deviceId, reply[1]))



# PART 3: COMMAND LINE INTERFACE

def printInfo(board):
    for d in board.devices:
    	print("Device '{}':".format(d.id))

        print("  Type:       {}".format(d.type))
    	print("  URI:        {}".format(d.uri))
    	print("  Addr table: {}".format(d.addressTable))
        print()

        if len(d.commands) == 0:
            print("  No commands")
        else:
            print("  Commands:")
            for command in d.commands:
                print("   * {}".format(command.id))
                if len(command.parameters) == 0:
                    print("     (No parameters)")
                else:
                    for p in command.parameters:
                        print("       {0} [{1}]".format(p.id, p.type))
            print()

        print("  {} FSMs".format(len(d.fsms)))

        print()
        print()
       

def convertStringToBool(value):
    valueMap = {'true': True, 't': True, '1': True,
             'false': False, 'f': False, '0': False,
             }

    if isinstance(value, bool):
        return value

    if not isinstance(value, basestring):
        raise ValueError('invalid literal for boolean. Not a string.')

    lower_value = value.lower()
    if lower_value in valueMap:
        return valueMap[lower_value]
    else:
        raise ValueError('invalid literal for boolean: "%s"' % value)


PARAM_TYPES = {
  "bool": convertStringToBool,
  "uint": int,
  "int": int,
  "float": float,
  "string": str
}


def inputCommandParameters(board, deviceId, commandId):
    device  = next((x for x in board.devices if x.id == deviceId), None)
    command = next((x for x in device.commands if x.id == commandId), None)

    parameters = {}
    if len(command.parameters) != 0:
        print("Please enter values for the following parameters ...")

        for param in command.parameters:
            value = PARAM_TYPES[param.type](raw_input("  {} [{}]: ".format(param.id, param.type)))
            parameters[param.id] = value
    return parameters


# Copied from v3 of promt_toolkit
class NestedCompleter(Completer):
    """
    Completer which wraps around several other completers, and calls any the
    one that corresponds with the first word of the input.
    By combining multiple `NestedCompleter` instances, we can achieve multiple
    hierarchical levels of autocompletion. This is useful when `WordCompleter`
    is not sufficient.
    If you need multiple levels, check out the `from_nested_dict` classmethod.
    """

    def __init__(self, options, ignore_case=True):

        self.options = options
        self.ignore_case = ignore_case

    def __repr__(self):
        return "NestedCompleter(%r, ignore_case=%r)" % (self.options, self.ignore_case)

    @classmethod
    def from_nested_dict(cls, data):
        """
        Create a `NestedCompleter`, starting from a nested dictionary data
        structure, like this:
        .. code::
            data = {
                'show': {
                    'version': None,
                    'interfaces': None,
                    'clock': None,
                    'ip': {'interface': {'brief'}}
                },
                'exit': None
                'enable': None
            }
        The value should be `None` if there is no further completion at some
        point. If all values in the dictionary are None, it is also possible to
        use a set instead.
        Values in this data structure can be a completers as well.
        """
        options = {}
        for key, value in data.items():
            if isinstance(value, Completer):
                options[key] = value
            elif isinstance(value, dict):
                options[key] = cls.from_nested_dict(value)
            elif isinstance(value, set):
                options[key] = cls.from_nested_dict({item: None for item in value})
            else:
                assert value is None
                options[key] = None

        return cls(options)

    def get_completions(self, document, complete_event):
        # Split document.
        text = document.text_before_cursor.lstrip()
        stripped_len = len(document.text_before_cursor) - len(text)

        # If there is a space, check for the first term, and use a
        # subcompleter.
        if " " in text:
            first_term = text.split()[0]
            completer = self.options.get(first_term)

            # If we have a sub completer, use this for the completions.
            if completer is not None:
                remaining_text = text[len(first_term) :].lstrip()
                move_cursor = len(text) - len(remaining_text) + stripped_len

                new_document = Document(
                    remaining_text,
                    cursor_position=document.cursor_position - move_cursor,
                )

                for c in completer.get_completions(new_document, complete_event):
                    yield c

        # No space in the input: behave exactly like `WordCompleter`.
        else:
            completer = WordCompleter(
                list(self.options.keys()), ignore_case=self.ignore_case, sentence=True
            )
            for c in completer.get_completions(document, complete_event):
                yield c


def createCompleter(board):
    completerMap = {}
    completerMap['exit'] = None
    completerMap['info'] = None
    completerMap['run-command'] = {x.id: {y.id for y in x.commands} for x in board.devices}
    completerMap['engage-fsm'] = {x.id: {y.id for y in x.fsms} for x in board.devices}
    completerMap['reset-fsm'] = completerMap['engage-fsm']
    completerMap['disengage-fsm'] = completerMap['engage-fsm']

    return NestedCompleter.from_nested_dict(completerMap)


# Class for validating inputs (e.g. checks that supplied IDs match those of registered devices/commands/FSMs)
class CustomValidator(Validator):

    def __init__(self, board):
    	self._board = board

    def validate(self, document):
        words = shlex.split(document.text)
        if words[0] not in ['exit', 'info', 'run-command', 'engage-fsm', 'reset-fsm', 'disengage-fsm', 'run-transition']:
            raise ValidationError(message='Invalid keyword "{}" given'.format(words[0]), cursor_position=0)

        if words[0] not in ['exit', 'info']:
            if len(words) == 1:
                raise ValidationError(message='Device ID not specified')
            if words[1] not in [x.id for x in self._board.devices]:
                raise ValidationError(message='"{}" does not match ID of any known devices'.format(words[1]))

            if len(words) == 2:
                if words[0] == 'run-command':
                    descr = 'Command'
                elif words[0] == 'run-transition':
                    descr = 'Transition'
                else:
                    descr = 'FSM'
                raise ValidationError(message=descr + ' ID not specified'.format())

            device = next((x for x in self._board.devices if x.id == words[1]), None)
            if (words[0] == 'run-command') and (words[2] not in [x.id for x in device.commands]):
                raise ValidationError(message='"{}" does not match ID of any registered commands'.format(words[2]))
            if words[0].endswith('-fsm') and (words[2] not in [x.id for x in device.fsms]):
                raise ValidationError(message='"{}" does not match ID of any registered FSMs'.format(words[2]))

        expectedNumWords = 1 if words[0] in ['exit', 'info'] else 3
        if len(words) > expectedNumWords:
            raise ValidationError(message="More words ({}) written in console than required ({})".format(len(words), expectedNumWords))




if __name__ == '__main__':

    if len(sys.argv) == 1:
        hostname = 'localhost'
        port = 12345
    elif len(sys.argv) == 3:
        hostname = sys.argv[1]
        port = int(sys.argv[2])
    else:
        print("ERROR: Invalid number of arguments supplied.")
        sys.exit(1)

    print(__doc__)

    print("Connecting to control app at {}:{}".format(hostname, port))
    socket = connect(hostname, port)
    board = getInfo(socket)
    print()

    session = PromptSession(message='> ', completer=createCompleter(board), validator=CustomValidator(board), validate_while_typing=False)

    while True:
        x = shlex.split(session.prompt())

        if len(x) == 0:
            continue

        elif x == ['info']:
            printInfo(board)

        elif x[0] == 'run-command':
            runCommand(socket, x[1], x[2], inputCommandParameters(board, x[1], x[2]))

        elif x[0] == 'engage-fsm':
            engageFSM(socket, x[1], x[2])

        elif x[0] == 'reset-fsm':
            resetFSM(socket, x[1], x[2])

        elif x[0] == 'disengage-fsm':
            disengageFSM(socket, x[1], x[2])

        elif x == ['exit']:
            break


