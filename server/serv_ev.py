async_mode = None

from flask import Flask, request, send_from_directory
from flask_socketio import SocketIO, emit

app = Flask(__name__, static_folder='./front_end/public')
#app.wsgi_app = socketio.WSGIApp(sio, app.wsgi_app)
socketio = SocketIO(app)

car_info = {}
car_info['id'] = 1
car_info['px'] = 2
car_info['py'] = 2
car_info['v'] = 2
car_info['th'] = 2

@app.route('/public/<path:path>')
def index(path):
    print("Get root")
    print(path)
    #return 'ok'
    return send_from_directory(app.static_folder, path)

@app.route('/esp_data')
def esp():
    print("Get esp")
    car_info['id'] = request.args.get('id')
    car_info['px'] = request.args.get('px')
    car_info['py'] = request.args.get('py')
    car_info['v'] = request.args.get('v')
    car_info['th'] = request.args.get('th')
    '''
    car_info['id'] = 1
    car_info['px'] += 2
    car_info['py'] += 2
    car_info['v'] += 2
    car_info['th'] += 2
    '''
    return 'ok'

@socketio.on('connect', namespace='/front')
def connect():
    print('connect ')

@socketio.on('front_update', namespace='/front' )
def front_update(data):
    #print('front_update ', data)
    '''
    car_info['id'] = 1
    car_info['px'] += 2
    car_info['py'] += 2
    car_info['v'] += 2
    car_info['th'] += 2
    '''
    emit('back_update', car_info, namespace='/front')

@socketio.on('disconnect', namespace='/front')
def disconnect():
    print('disconnect ')

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=10418, debug=False)
