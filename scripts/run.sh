echo "abc" >> test_data/input/abc
echo "tsetse123" >> test_data/input/8/14/15

./build/linuxTimeMachineBackend test_data/input/ test_data/output -cr 20
