## Usage

## Setup

First, set the pintos path in init.sh according to its true location on your system.
Then, run the setup.sh script.

```
vi init.sh
./setup.sh
```

This automatically sets up pintos and adds these scripts to your PATH.


## The `check` command

This command allows you to select and run inidiviual tests via index.
It also automatically sends a Slack notification of the results via a webhook.
To enable this, add a file named KEY in the current directory containing the url of the webhook.

Usage: check *project* [*test_index* | all].

Just try this (after setup)

```
check threads
```


## Appendix A. Source script vs ./script

Normally this is how you would run a script

```bash
source init.sh
```

This runs all the commands in init.sh as if you'd just typed the commands in your terminal.
However, some of the scripts like setup.sh have stuff like cd that shouldn't be executed in your shell.
Hence, we execute them in subshells like so:

```bash
./init.sh
```

All scripts in this folder should be run using ./script.
It is recommended, however, that you simply use `setup.sh`.

