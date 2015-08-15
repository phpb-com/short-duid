var Benchmark = require('benchmark'),
    ShortDUID = require('../.');

var suit = new Benchmark.Suite;

var duid = ShortDUID.init(0, "b130389689f522fa8b6664eb291083551ff0c00a4cf5a4905fdee8cd9063e55a", 1433116800000);
var duid_small_salt = ShortDUID.init(0, "a", 1433116800000);

suit.add('single DUIDInt generation', function () {
    duid.getDUIDInt(1);
});

suit.add('batch of 10 DUIDInt generation (multiply by 10 to get IDs per second)', function () {
    duid.getDUIDInt(10);
});

suit.add('batch of 1024 DUIDInt generation (multiply by 1024 to get IDs per second)', function () {
    duid.getDUIDInt(1024);
});

suit.add('batch of 4096 DUIDInt generation (multiply by 4096 to get IDs per second)', function () {
    duid.getDUIDInt(4096);
});

suit.add('batch of 8192 DUIDInt generation (multiply by 8192 to get IDs per second)', function () {
    duid.getDUIDInt(8192);
});

suit.add('single DUID generation', function () {
    duid.getDUID(1);
});

suit.add('batch of 10 DUID generation (multiply by 10 to get IDs per second)', function () {
    duid.getDUID(10);
});

suit.add('batch of 64 DUID generation (multiply by 64 to get IDs per second)', function () {
    duid.getDUID(64);
});

suit.add('single DUID generation (1 character salt)', function () {
    duid_small_salt.getDUID(1);
});

suit.add('batch of 10 DUID generation (1 character salt)', function () {
    duid_small_salt.getDUID(10);
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
