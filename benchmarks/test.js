var Benchmark = require('benchmark'),
    ShortDUID = require('../.');

var suit = new Benchmark.Suite;

var duid = ShortDUID.init(0, "b130389689f522fa8b6664eb291083551ff0c00a4cf5a4905fdee8cd9063e55a", 1433116800000);

suit.add('single DUIDInt generation', function () {
    duid.getDUIDInt(1);
});

suit.add('batch of 10 DUIDInt generation', function () {
    duid.getDUIDInt(10);
});

suit.add('single DUID generation', function () {
    duid.getDUID(1);
});

suit.add('batch of 10 DUID generation', function () {
    duid.getDUID(10);
});

suit.add('singe getRandomAPIKey generation', function () {
    duid.getRandomAPIKey();
});

suit.add('single getRandomPassword generation', function () {
    duid.getRandomPassword();
});

suit.on('cycle', function(event) {
      console.log(String(event.target));
})

// run async
suit.run({ 'async': true });
