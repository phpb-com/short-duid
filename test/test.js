var duid = require( '../index' );
var test = require( "unit.js" );
var async = require( "async" );
var BN = require( "bn.js" );
var _ = require('lodash');

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
  var epoch_start = 1433116800 * 1000; //Mon, 01 Jun 2015 00:00:00 GMT
  var salt = '39622feb2b3e7aa7208f50f45ec36fd513baadad6977b53295a3b28aeaed4a54'; //dd if=/dev/random bs=1 count=102400 2>/dev/null| sha256sum

  var duid_instance1 = new init( 123, salt, epoch_start );
  var duid_instance2 = new init( 12, salt, epoch_start );

  var random_integer1 = _.random(1, 1000 * 1000);
  var random_integer2 = _.random(1, 1000 * 1000);
  var random_integer3 = _.random(1, 1000 * 1000);

  describe( '#hashidEncode() and #hashidDecode()', function () {

    it( 'should produce identical hashids from both instances for: ' + random_integer1, function () {
      test.value( duid_instance1.hashidEncode( [ random_integer1 ] ) )
          .isIdenticalTo( duid_instance2.hashidEncode( [ random_integer1 ] ) );
    } );

    it( 'should produce different hashids for two different integers: ' + random_integer1 + ' and ' + random_integer2, function () {
      test.value( duid_instance1.hashidEncode( [ random_integer1 ] ) )
          .isNotEqualTo( duid_instance1.hashidEncode( [ random_integer2 ] ) );
    } );

    it( 'decode should return same integer given output of encode as argument passed to encode: ' + random_integer3, function () {
      test.value( duid_instance2.hashidDecode( duid_instance1.hashidEncode( [ random_integer3 ] ) )[ 0 ] )
          .isEqualTo( random_integer3 );
    } );

    it( 'decode should return same array of integers given output of encode as argument passed to encode: ' + [ random_integer1, random_integer2, random_integer3 ], function () {
      test.value( duid_instance2.hashidDecode( duid_instance1.hashidEncode( [ random_integer1, "" + random_integer2, random_integer3 ] ) ) )
          .isArray()
          .is( [ "" + random_integer1, "" + random_integer2, "" + random_integer3 ] );
    } );

    it( 'should return hashid that is equal to "LeGxr" given [123456] as argument', function () {
      test.value( duid_instance1.hashidEncode( [ 123456 ] ) )
          .is( 'LeGxr' );
    } );

    it( 'should return hashid that is equal to [ "123456" ] given "LeGxr" as argument', function () {
      test.value( duid_instance2.hashidDecode( "LeGxr" ) )
          .is( [ "123456" ] );
    } );

    it( 'should return hashid that is equal to "reG4QhO4NCpm" given [123456,7890,123] as argument', function () {
      test.value( duid_instance1.hashidEncode( [ "123456", 7890, "123" ] ) )
          .is( 'reG4QhO4NCpm' );
    } );

    it( 'should return hashid that is equal to [123456,7890,123] given "reG4QhO4NCpm" as argument', function () {
      test.value( duid_instance2.hashidDecode( "reG4QhO4NCpm" ) )
          .is( [ "123456", "7890", "123" ] );
    } );

    it( 'should return different hashids given same value and different salt', function () {
      var duid_tmp1 = new init( 0, "salt#1", 0 );
      var duid_tmp2 = new init( 0, "salt#2", 0 );
      test.string( duid_tmp1.hashidEncode( [ 123456 ] ) ).isEqualTo( duid_tmp1.hashidEncode( [ 123456 ] ) );
      test.string( duid_tmp2.hashidEncode( [ 123456 ] ) ).isEqualTo( duid_tmp2.hashidEncode( [ 123456 ] ) );
      test.string( duid_tmp1.hashidEncode( [ 123456 ] ) ).isNotEqualTo( duid_tmp2.hashidEncode( [ 123456 ] ) );
    } );

  } );

  describe( '#getRandomAPIKey()', function () {

    it( 'should return random API key 64 characters long', function () {
      test.string( duid_instance1.getRandomAPIKey() ).hasLength( 64 );
    } );

    it( 'should return random API key each time called, should not be equal', function () {
      test.string( duid_instance1.getRandomAPIKey() )
          .isNotEqualTo( duid_instance2.getRandomAPIKey() )
          .isNotEqualTo( duid_instance1.getRandomAPIKey() )
          .isNotEqualTo( duid_instance2.getRandomAPIKey() )
          .isNotEqualTo( duid_instance1.getRandomAPIKey() )
          .isNotEqualTo( duid_instance2.getRandomAPIKey() )
          .isNotEqualTo( duid_instance1.getRandomAPIKey() )
          .isNotEqualTo( duid_instance2.getRandomAPIKey() )
          .isNotEqualTo( duid_instance1.getRandomAPIKey() )
          .isNotEqualTo( duid_instance2.getRandomAPIKey() );
    } );

  } );

  describe( '#getRandomPassword()', function () {

    it( 'should return random password 16 characters long', function () {
      test.string( duid_instance1.getRandomPassword() ).hasLength( 16 );
    } );

    it( 'should return random password each time called, should not be equal', function () {
      test.string( duid_instance1.getRandomPassword() )
          .isNotEqualTo( duid_instance2.getRandomPassword() )
          .isNotEqualTo( duid_instance1.getRandomPassword() )
          .isNotEqualTo( duid_instance2.getRandomPassword() )
          .isNotEqualTo( duid_instance1.getRandomPassword() )
          .isNotEqualTo( duid_instance2.getRandomPassword() )
          .isNotEqualTo( duid_instance1.getRandomPassword() )
          .isNotEqualTo( duid_instance2.getRandomPassword() )
          .isNotEqualTo( duid_instance1.getRandomPassword() )
          .isNotEqualTo( duid_instance2.getRandomPassword() );
    } );

  } );

  describe( '#getEpochStart()', function () {

    it( 'should return set epoch start, for instance #1: ' + epoch_start, function () {
      test.value( duid_instance1.getEpochStart() )
          .isEqualTo( epoch_start );
    } );

    it( 'should return set epoch start, for instance #2: ' + epoch_start, function () {
      test.value( duid_instance2.getEpochStart() )
          .isEqualTo( epoch_start );
    } );

    it( 'instance #1 and instance #2 should return same epoch start: ' + epoch_start, function () {
      test.value( duid_instance1.getEpochStart() ).isIdenticalTo( duid_instance2.getEpochStart() );
    } );

    it( 'should reset custom epoch to zero if given one larger than real epoch', function () {
      var custom_epoch_overflow = (new Date()).getTime() + _.random(2000, 1000 * 1000);
      var duid_instance_overflow = new init( 0, salt, custom_epoch_overflow );
      test.value( duid_instance_overflow.getEpochStart() ).isEqualTo( 0 );
      test.value( duid_instance_overflow.getEpochStart() ).isNotEqualTo( custom_epoch_overflow );
    } );

    it( 'should accept custom epoch that is even 1 millisecond in the past', function () {
      var custom_epoch_near = (new Date()).getTime() - _.random(1, 2);
      var duid_instance_near = new init( 0, salt, custom_epoch_near );
      test.value( duid_instance_near.getEpochStart() ).isEqualTo( custom_epoch_near );
    } );

  } );

  describe( '#getSalt()', function () {

    it( 'should return set salt, for instance #1: ' + salt, function () {
      test.string( duid_instance1.getSalt() ).isIdenticalTo( salt );
    } );

    it( 'should return set salt, for instance #2: ' + salt, function () {
      test.string( duid_instance2.getSalt() ).isIdenticalTo( salt );
    } );

    it( 'instance #1 and instance #2 should return same salt: ' + salt, function () {
      test.string( duid_instance1.getSalt() ).isIdenticalTo( duid_instance2.getSalt() );
    } );

  } );

  describe( '#getShardID()', function () {

    var duid_instance_shardid_overflow = new init( 1024, salt, 0 );

    it( 'should overflow if shard_id is set to integer that does not fit in 10 bits: 1024 --> 0', function () {
      test.number( duid_instance_shardid_overflow.getShardID() ).is( 0 );
    } );

    it( 'should return set shard id for instance #1: 123', function () {
      test.number( duid_instance1.getShardID() ).is( 123 );
    } );

    it( 'should return set shard id for instance #2: 12', function () {
      test.number( duid_instance2.getShardID() ).is( 12 );
    } );

    it( 'should return different shard ids for instance #1 and instance #2', function () {
      test.number( duid_instance2.getShardID() ).isNot( duid_instance1.getShardID() );
    } );

  } );

  describe( '#getDUID()', function () {
    var tests = [ {
      args: 1,
      expected: 1
    }, {
      args: 0,
      expected: 0
    }, {
      args: 8192,
      expected: 8192
    }, {
      args: 8193,
      expected: 1
    } ];

    tests.forEach( function ( iter ) {
      it( 'Asked for ' + iter.args + ' DUIDs, correctly returns ' + iter.expected + ' DUIDs', function () {
        test.array( duid_instance1.getDUID( iter.args ) ).hasLength( iter.expected );
      } );
    } );

    it( 'should have no duplicates in the returned arrays, 8192 IDs each, and combined.', function () {
      var res1 = duid_instance1.getDUID( 8192 );
      var res2 = duid_instance2.getDUID( 8192 );
      var res3 = duid_instance1.getDUID( 8192 );
      test.array( _.uniq( res1 ) ).is( res1 );
      test.array( _.uniq( res2 ) ).is( res2 );
      test.array( _.uniq( res3 ) ).is( res3 );
      test.array( _.uniq( _.flattenDeep( [ res1, res2, res3 ] ) ) ).is( _.flattenDeep( [ res1, res2, res3 ] ) );
    } );

  } );

  describe( '#getDUIDInt()', function () {
    var tests = [ {
      args: 1,
      expected: 1
    }, {
      args: 0,
      expected: 0
    }, {
      args: 8192,
      expected: 8192
    }, {
      args: 8193,
      expected: 1
    } ];

    tests.forEach( function ( iter ) {
      it( 'Asked for ' + iter.args + ' Int DUIDs, correctly returns ' + iter.expected + ' Integer DUIDs', function () {
        test.array( duid_instance1.getDUID( iter.args ) ).hasLength( iter.expected );
      } );
    } );

    it( 'should have no duplicates in the returned arrays, 8192 IDs each, and combined.', function () {
      var res1 = duid_instance1.getDUIDInt( 8192 );
      var res2 = duid_instance2.getDUIDInt( 8192 );
      var res3 = duid_instance1.getDUIDInt( 8192 );
      test.array( _.uniq( res1 ) ).is( res1 );
      test.array( _.uniq( res2 ) ).is( res2 );
      test.array( _.uniq( res3 ) ).is( res3 );
      test.array( _.uniq( _.flattenDeep( [ res1, res2, res3 ] ) ) ).is( _.flattenDeep( [ res1, res2, res3 ] ) );
    } );

  } );

  describe( 'DUID with drifting time', function () {

    var duid_instance3 = new init( 123, salt, epoch_start );
    var id1 = new BN( duid_instance3.getDUIDInt( 1 )[ 0 ], 10 );
    var drift = duid_instance3.driftTime( Math.random() * 1000 * 10 * -1 | 0 );
    var id2 = new BN( duid_instance3.getDUIDInt( 4096 )[ 4095 ], 10 ); //Need to rollover sequence
    var curr_ms_time = new BN( duid_instance3.getCurrentTimeMs(), 10 );

    it( 'should generate ID with ' + drift + ' millisecond drift into the past from now( ' + curr_ms_time + ' ), ' + id1 + ' should be numerically smaller than ' + id2, function () {
      test.bool( id2.cmp( id1 ) === 1 ).isTrue();
    } );

    it( 'should consistently generate unique IDs even when time is drifting backwards constantly', function () {
      drift = duid_instance3.driftTime( 0 ); //Reset drift back to 0
      var duids = [];
      for(var i = 0; i < 10; ++i) {
        duid_instance3.driftTime( drift );
        var start = (new Date()).getTime();
        duids[i] = duid_instance3.getDUIDInt( 8192 );
        drift = start - (new Date()).getTime();
        test.number( drift ).isLessThan( 0 );
        test.array( duids[i] ).hasLength( 8192 );
        test.array( _.uniq( duids[i] ) ).is( duids[i] );
      }
      var merged_uniq = _.uniq( _.flattenDeep( duids ) );
      test.array( merged_uniq ).is( _.flattenDeep( duids ) );
      test.array( merged_uniq ).hasLength( 8192 * 10 );
    } );

  } );

} );

// vim: syntax=cpp11:ts=2:sw=2
