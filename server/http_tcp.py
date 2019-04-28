from flask import Flask, render_template, request
import random, socket, threading
import _thread

#tcp server
TCP_IP = '0.0.0.0'
TCP_PORT = 8888
BUFFER_SIZE  = 20
conn = None
def launchServer():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind((TCP_IP, TCP_PORT))
    s.listen(1)

    print('waiting for connection')
    global conn
    conn, addr = s.accept()

    print ('Connection address:', addr)


#flask app
app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def index():
    # global conn
    if request.method == 'POST':
        if request.form['submit'] == 'On':
            if conn == None:
                print(conn)
                return "Chưa có kết nối"
            else:
                # conn.send(b'button1')
                conn.send(str.encode('l'))
            return '''
                    <title>What would you like to do?</title>
                    <form action="" method="post">
                    <br><br>
                    <input type="submit" name="submit" value="On">
                    <br><br>
                    <input type="submit" name="submit" value="Off">
                    </form>
                    '''
        elif request.form['submit'] == 'Off':
            # print(conn)
            if conn == None:
                return "Chưa có kết nối"
            else:
                conn.send(str.encode('z'))
            return '''
                    <title>What would you like to do?</title>
                    <form action="" method="post">
                    <br><br>
                    <input type="submit" name="submit" value="On">
                    <br><br>
                    <input type="submit" name="submit" value="Off">
                    </form>
                    '''
        else:
            pass

    if request.method == 'GET':
        return '''
        <title>What would you like to do?</title>
        <form action="" method="post">
        <br><br>
        <input type="submit" name="submit" value="On">
        <br><br>
        <input type="submit" name="submit" value="Off">
        </form>
        '''

# t = threading.Thread(target=launchServer)
# t.daemon = True
# t.start()
_thread.start_new_thread(launchServer,())
# app.listen(5000)
app.run(debug=True,host="0.0.0.0",port=80)