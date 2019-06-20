from aiohttp import web
import socketio

sio = socketio.AsyncServer()
app = web.Application()
sio.attach(app)

FILE_PATH = './front_end/public'
MAIN_INDEX = '/index.html'
async def index(request):
    print("Get new req")
    """Serve the client-side application."""
    with open(FILE_PATH + MAIN_INDEX) as f:
        return web.Response(text=f.read(), content_type='text/html')

async def esp(request):
    print("Get new esp")
    paras = request.rel_url.query['id']
    print(paras)
    return web.Response(text='ok')
    #return web.Response()

@sio.event
def connect(sid, environ):
    print("connect ", sid)

@sio.event
async def message(sid, data):
    print("message ", data)
    await sio.emit('reply', room=sid)

@sio.event
def disconnect(sid):
    print('disconnect ', sid)

app.router.add_static('/public', './front_end/public')
app.router.add_get('/esp_data', esp, allow_head=False)
app.router.add_get('/public/{name}', index)
app.router.add_route('*', '/esp_data', esp)


if __name__ == '__main__':
    web.run_app(app, host='localhost', port=10418)
    print("Server running...")
