const addon = require('../build/Release/krooms-server-native');

function KroomsServer(name) {
    this.greet = function(str) {
        return _addonInstance.greet(str);
    }
    this.start = function() {
        _addonInstance.start();
    }
    this.stop = function() {
        _addonInstance.stop();
    }
    this.updateMessage = function(message) {
        _addonInstance.updateMessage(message);
    }

    var _addonInstance = new addon.KroomsServer(name);
}

module.exports = KroomsServer;
