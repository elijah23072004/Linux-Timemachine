timerLoc=~/.config/systemd/user/

cp ./data/TimeMachine.service ${timerLoc}TimeMachine.service
cp ./data/TimeMachine.timer ${timerLoc}TimeMachine.timer

#echo $timerLoc


systemctl --user daemon-reload
systemctl --user start TimeMachine.timer

