// 加载从 build/lib 目录下的 .node 文件
const path = require('path');
const addon = require(path.join(__dirname, '../../../build/lib/krooms-receive-lib.node'));

class KroomsReceive {
    constructor(name = 'default-client') {
        this._addonInstance = new addon.KroomsReceiveWrapper(name);
    }

    greet(str) {
        return this._addonInstance.greet(str);
    }

    start(callback) {
        return this._addonInstance.start(callback);
    }

    stop() {
        return this._addonInstance.stop();
    }
}

module.exports = KroomsReceive;