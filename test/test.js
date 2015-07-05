var duid = require( '../index' );
var assert = require( "assert" );
var async = require( "async" );
var bignum = require( "bignum" );

var check_duplicates = function ( arr ) {
  arr.sort();
  for ( var i = 1; i < arr.length; i++ ) {
    if ( arr[ i - 1 ] === arr[ i ] )
      return false;
  }
  return true;
};

describe( 'Short DUID', function () {

  var init = duid.init;
  var epoch_start = 1433116800; //Mon, 01 Jun 2015 00:00:00 GMT
  var salt = '39622feb2b3e7aa7208f50f45ec36fd513baadad6977b53295a3b28aeaed4a54'; //dd if=/dev/random bs=1 count=102400 2>/dev/null| sha256sum

  var duid_instance1 = new init( 123, salt, epoch_start );
  var duid_instance2 = new init( 12, salt, epoch_start );

  var random_integer1 = Math.random() * 1000 * 1000 * 1000 | 0;
  var random_integer2 = Math.random() * 1000 * 1000 * 1000 | 0;
  var random_integer3 = Math.random() * 1000 * 1000 * 1000 | 0;

  describe( '#hashidEncode() and #hashidDecode()', function () {

    it( 'should produce identical hashids from both instances for: ' + random_integer1, function () {
      assert.equal( duid_instance1.hashidEncode( [ random_integer1 ] ), duid_instance2.hashidEncode( [ random_integer1 ] ) );
    } );

    it( 'should produce different hashids for two different integers: ' + random_integer1 + ' and ' + random_integer2, function () {
      assert.notEqual( duid_instance1.hashidEncode( [ random_integer1 ] ), duid_instance1.hashidEncode( [ random_integer2 ] ) );
    } );

    it( 'decode should return same integer given output of encode as argument passed to encode: ' + random_integer3, function () {
      assert.equal( duid_instance2.hashidDecode( duid_instance1.hashidEncode( [ random_integer3 ] ) )[ 0 ], random_integer3 );
    } );

    it( 'decode should return same array of integers given output of encode as argument passed to encode: ' + [ random_integer1, random_integer2, random_integer3 ], function () {
      assert.deepEqual( duid_instance2.hashidDecode( duid_instance1.hashidEncode( [ random_integer1, random_integer2, random_integer3 ] ) ), [ random_integer1, random_integer2, random_integer3 ] );
    } );

    it( 'should return hashid that is equal to "LeGxr" given [123456] as argument', function () {
      assert.equal( 'LeGxr', duid_instance1.hashidEncode( [ 123456 ] ) );
    } );

    it( 'should return hashid that is equal to [123456] given "LeGxr" as argument', function () {
      assert.deepEqual( [ 123456 ], duid_instance2.hashidDecode( "LeGxr" ) );
    } );

    it( 'should return hashid that is equal to "reG4QhO4NCpm" given [123456,7890,123] as argument', function () {
      assert.equal( 'reG4QhO4NCpm', duid_instance1.hashidEncode( [ 123456, 7890, 123 ] ) );
    } );

    it( 'should return hashid that is equal to [123456,7890,123] given "reG4QhO4NCpm" as argument', function () {
      assert.deepEqual( [ 123456, 7890, 123 ], duid_instance2.hashidDecode( "reG4QhO4NCpm" ) );
    } );

  } );

  describe( '#getRandomAPIKey()', function () {

    it( 'should return random API key 64 characters long', function () {
      assert.equal( duid_instance1.getRandomAPIKey().length, 64 );
    } );

    it( 'should return random API key each time called, should not be equal', function () {
      assert.notEqual( duid_instance1.getRandomAPIKey(), duid_instance1.getRandomAPIKey() );
    } );

  } );

  describe( '#getRandomPassword()', function () {

    it( 'should return random password 16 characters long', function () {
      assert.equal( duid_instance1.getRandomPassword().length, 16 );
    } );

    it( 'should return random password each time called, should not be equal', function () {
      assert.notEqual( duid_instance1.getRandomPassword(), duid_instance1.getRandomPassword() );
    } );

  } );

  describe( '#getEpochStart()', function () {

    it( 'should return set epoch start, for instance #1: ' + epoch_start, function () {
      assert.equal( epoch_start, duid_instance1.getEpochStart() );
    } );

    it( 'should return set epoch start, for instance #2: ' + epoch_start, function () {
      assert.equal( epoch_start, duid_instance2.getEpochStart() );
    } );

    it( 'instance #1 and instance #2 should return same epoch start: ' + epoch_start, function () {
      assert.equal( duid_instance1.getEpochStart(), duid_instance2.getEpochStart() );
    } );

  } );

  describe( '#getSalt()', function () {

    it( 'should return set salt, for instance #1: ' + salt, function () {
      assert.equal( salt, duid_instance1.getSalt() );
    } );

    it( 'should return set salt, for instance #2: ' + salt, function () {
      assert.equal( salt, duid_instance2.getSalt() );
    } );

    it( 'instance #1 and instance #2 should return same salt: ' + salt, function () {
      assert.equal( duid_instance1.getSalt(), duid_instance2.getSalt() );
    } );

  } );

  describe( '#getShardID()', function () {

    it( 'should return set shard id for instance #1: 123', function () {
      assert.equal( 123, duid_instance1.getShardID() );
    } );

    it( 'should return set shard id for instance #2: 12', function () {
      assert.equal( 12, duid_instance2.getShardID() );
    } );

  } );

  describe( '#getDUID()', function () {
    var tests = [ {
      args: 1,
      expected: 1
    }, {
      args: 8192,
      expected: 8192
    }, {
      args: 8193,
      expected: 1
    } ];

    tests.forEach( function ( test ) {
      it( 'Asked for ' + test.args + ' DUIDs, correctly returns ' + test.expected + ' DUIDs', function () {
        var res = duid_instance1.getDUID( test.args );
        assert.equal( res.length, test.expected );
      } );
    } );

    it( 'should have no duplicates in the returned arrays, 8192 IDs each, and combined.', function () {
      var res1 = duid_instance1.getDUID( 8192 );
      var res2 = duid_instance2.getDUID( 8192 );
      assert.ok( check_duplicates( res1 ) );
      assert.ok( check_duplicates( res2 ) );
      assert.ok( check_duplicates( res1.concat( res2 ) ) );
    } );

  } );

  describe( '#getDUIDInt()', function () {
    var tests = [ {
      args: 1,
      expected: 1
    }, {
      args: 8192,
      expected: 8192
    }, {
      args: 8193,
      expected: 1
    } ];

    tests.forEach( function ( test ) {
      it( 'Asked for ' + test.args + ' Int DUIDs, correctly returns ' + test.expected + ' Integer DUIDs', function () {
        var res = duid_instance1.getDUIDInt( test.args );
        assert.equal( res.length, test.expected );
      } );
    } );

    it( 'should have no duplicates in the returned arrays, 8192 IDs each, and combined.', function () {
      var res1 = duid_instance1.getDUIDInt( 8192 );
      var res2 = duid_instance2.getDUIDInt( 8192 );
      assert.ok( check_duplicates( res1 ) );
      assert.ok( check_duplicates( res2 ) );
      assert.ok( check_duplicates( res1.concat( res2 ) ) );
    } );

  } );

  describe( 'DUID with drifting time', function () {

    var duid_instance3 = new init( 123, salt, epoch_start );
    var id1 = bignum( duid_instance3.getDUIDInt( 1 )[ 0 ], 10 );
    var drift = duid_instance3.driftTime( Math.random() * 1000 * 10 * -1 | 0 );
    var id2 = bignum( duid_instance3.getDUIDInt( 4096 )[ 4095 ], 10 ); //Need to rollover sequence
    var curr_ms_time = bignum( duid_instance3.getCurrentTimeMs(), 10 );
    duid_instance3.driftTime( 0 ); //Reset drift back to 0

    it( 'should generate ID with ' + drift + ' millisecond drift into the past from now( ' + curr_ms_time + ' ), ' + id1 + ' should be numerically smaller than ' + id2, function () {
      assert.ok( id2.gt( id1 ), id2 + ' > ' + id1 );
    } );

  } );

} );

// vim: syntax=cpp11:ts=2:sw=2
