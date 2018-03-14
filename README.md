# gdosc
This is a Godot game engine module for receiving and sending Open Sound Control messages [OSC](http://opensoundcontrol.org/introduction-osc).
It uses oscpack as OSC protocol implementation so it potentially it works on all platforms that can handle oscpack. However it has been tested only on Linux (and, for the moment, I am not interested in any other platforms).

## Installation

### Godot engine
First, you will need the Godot engine sources. Head over to [Godot compilation instructions](http://docs.godotengine.org/en/latest/development/compiling/) to get Godot building on your system (make sure it can build with `use_llvm=yes`).

Then copy or link `gdosc` to `godot/modules` and recompile Godot with `scons platform=x11 use_llvm=yes`

### deps

#### linux

If you wants to use clang, install it with:

`sudo apt install clang`

If you are using gcc, just skip the *use_llvm=yes* and it will be ok.

#### git

If you are not familiar with git, make sure it is installed:

`sudo apt install git`

Generate SSH key by following this [tutorial](https://www.siteground.com/kb/generate_ssh_key_in_linux/) and add it to your [github](https://help.github.com/articles/adding-a-new-ssh-key-to-your-github-account/) account - HTTPS links are not working with submodules!

Once done, use these 2 commands to get the repo up & running:

`git clone git@github.com:djiamnot/gdosc.git`

`git pull && git submodule init && git submodule update && git submodule status`

It will retrieve *oscpack* submodule.

### gdosc

Then, in your sources directory `git clone --recurse-submodules https://github.com/djiamnot/gdosc` and after that:

`cd godot`

`ln -s <path/to/gdosc/ modules/`

`scons platform=x11 use_llvm=yes`

### Example
Clone https://github.com/djiamnot/gdOscTest. See the README for information about the example.

## Known issues
At this time, gdosc supports only three data-types: _float_, _int_, and _string_. This is sufficient for me right now, but merge requests are welcome!

Theoretically, this should work on any OS but it has been developed and tested on Linux with Godot3.0

## Worlkflow

First you need to instantiate OSCListener class. In my current workflow, I attach a script to root node in the scene. OSCListener emits a signal `osdc_message` which you can pick up and do with what you want

```python

extends Spatial

signal ready()
signal osc_message()
var osc_ready = false

var osc_listener
# This is how we instantiate the main receiver class. It will probably exist in your scene and
# interested object will connect to its signal
func _ready():
    osc_listener  = OSCListener.new() # port 18002 by default
    #  port can be changed:
    # osc_listener.setPort(18004)

    # osc_message is a signal emitted on every reception of OSC message
    osc_liprobablementstener.connect("osc_message", self, "_on_osc_msg")
    # there is also 'osc_ready' signal for your convenience, emited as soon as the OSC receiver is
    # successfully instantiated

# I like to forward the message (at this time, I am considering this my top-level node and scenes
# will be instantiated below it)
func _on_osc_msg(val):
    emit_signal("osc_message", val)
```

So in the code above, pick it up and simply forward it so that scenes that I will instantiate under my root will subscribe to it. This is the kind of workflow that makes sense to me.

Then, something we can subscribe from any other node:

```python
var osc

func _ready():
    set_process_input(true)
    set_process(true)
    # here we get the node where OSC communication is received.
    # it emits a 'osc_message' signal
    osc = get_parent_spatial()
    osc.connect("osc_message", self, "_on_osc_message")

func _on_osc_message(val):
    print(get_name(), " ", val)
    # test for my name
    if val[0].right(1).match(get_name()):
        print("matching name -- _on_osc_message")
        if val[1] == "pos":
            translate(Vector3(val[2], val[3], val[4]))
```

### Sending messages

First of all, you need to add an **OSCtransmitter** node in your scene. It is located in the first level of the tree, between *HTTPRequest* and *ResourcePreloader*.

![OSCtransmitter in creation menu](https://frankiezafe.org/images/7/7c/Godot_gdosc_OSCtransmitter.png)

Once done, attach a script to it who looks like this:

```python
extends OSCtransmitter

signal exit()

func _ready():
	set_process(true)
	# initialisation of OSC sender, on port 25000 and with a buffer size of 1024
	init("localhost", 25000, 1024)
	framecount = 0
	pass

func _process(delta):
	# creation of the new message
	setAddress("/update")
	# appending data
	appendInt(framecount)
	# sending the message to the client
	sendMessage()
	# cleanup of the message
	reset()
	framecount += 1
	pass
```