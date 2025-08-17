const addon = require('../build/Release/krooms-client-native');

function KroomsClient(name) {
    this.greet = function(str) {
        return _addonInstance.greet(str);
    }

    var _addonInstance = new addon.KroomsClient(name);
}

module.exports = KroomsClient;
