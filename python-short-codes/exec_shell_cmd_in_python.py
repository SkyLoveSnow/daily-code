# test code for subprocess.popen
# 1. subprocess.call(args,*,stdin=None,stdout=None,stderr=None,shell=False)
#    |___运行args中的命令，等待完成，然后返回 returncode 属性
#    |___example:
# 			    |___ >>>subprocess.call(["ls","-l"])
# 			    |___ >>>0
# 			    |___ >>>a=subprocess.call("exit 1",shell=True) //指定用shell运行该命令
#			    |___ >>>a.real
#			    |___ >>>1
#2. subprocess.check_call(args,*,stdin=None,stdout=None,stderr=None,shell=False)
#   如果返回非零，则抛出异常CalldProcessError,异常中包含returncode 属性
#3. subprocess.check_out(args,*,stdin=None,stderr=None,shell=False,universal_newlines=False)
#   run command with args and return its output as a byte string
#   |___ >>> subprocess.check_out(["echo","hello world\n"])
#   |___ >>> hello world 
#   |___ >>> 
#   |___ >>> 0
#4. class:subprocess.Popen(args,bufsize=0,executable=None,stdin=None,stdout=None,stderr=None,
#					       preexec_fn=None,close_fds=False,shell=False,cwd=None,env=None,
#					       universal_newlines=False,startupinfo=None,creationflags=0)
# 	在new process中执行子程序，相当于调用os.execp()
#   instance method:
#					poll(): check terminate
#					wait(): wait for child process terminate
#					communicate(input=None): 和process进行交互，将数据发送到stdin,从stdout,stderr读取数据，方法返回(stdoutdata,stderrdata)
#					send_signal(signal)
#					terminate()
#					kill()
#	instance attr:
#					stdin: 如果stdin参数是subprocess.PIPE,则该属性是一个文件对象，用于给child process提供input数据
# 					stdout: 同上
#					stderr: 同上
#					pid:子进程的 ID
#					returncode: 进程返回码

#指定PIPE，则不会输出到stdout
import subprocess
process = subprocess.Popen(["ls","-l"],
							stdin=subprocess.PIPE,
							stdout=subprocess.PIPE,
							stderr=subprocess.PIPE)
output,err = process.communicate()