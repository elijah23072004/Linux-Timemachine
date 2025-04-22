timerLoc=~/.config/systemd/user/

cp ./installer/data/LinuxTimeMachine.service ${timerLoc}LinuxTimeMachine.service
cp ./installer/data/LinuxTimeMachine.timer ${timerLoc}LinuxTimeMachine.timer

#echo $timerLoc


systemctl --user daemon-reload
systemctl --user start LinuxTimeMachine.timer

