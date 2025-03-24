project('linuxTimeMachine', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++17'])

gtkmm_dep = dependency('gtkmm-4.0')


exe = executable('linuxTimeMachineBackend', 'src/linuxtimemachine.cpp',
  install : true)

exe1 = executable('randomFileSystemGenerator', 'src/testing/randomFileSystemGenerator.cpp')

cpp_sources2 = [
  'src/frontendApplication/main.cpp',
  'src/frontendApplication/backupappwindow.cpp',
  'src/frontendApplication/backupapplication.cpp']
exe2 = executable('linuxTimeMachineApp', cpp_sources2,dependencies: gtkmm_dep)
test('basic', exe)

exe3 = executable('createRandomDifferences', 'src/testing/createRandomDifferences.cpp')
