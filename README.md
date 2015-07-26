# Short Distributed ID generator module

This module was inspired by [icicle](https://github.com/intenthq/icicle) and [snowflake](https://github.com/twitter/snowflake). The ideas is to be able to generate non-colliding, URL friendly, and relatively short IDs that could be used in any application that requires to create URIs for arbitrary resources.

Looking around for what is available, I failed to find anything that would be simple and easy to implement. As a result, this module was born.

The id is a 64bit unsigned integer with 42 bits used for current timestamp in milliseconds, 10 bits used for shard id, and final 12 bits are used for revolving sequence.

| timestamp_ms | shard_id | sequence |
|:---:|:---:|:---:|
| 42bit | 10bit | 12bit |

## short-duid

[![Build Status](https://travis-ci.org/phpb-com/short-duid.svg)](https://travis-ci.org/phpb-com/short-duid)
[![npm version](https://img.shields.io/npm/v/short-duid.svg?style=flat-square)](https://www.npmjs.com/package/short-duid)
[![npm downloads](https://img.shields.io/npm/dm/short-duid.svg?style=flat-square)](https://www.npmjs.com/package/short-duid)
[![Dependency Status](https://gemnasium.com/4f0db98c7a23b31f50732ee33bc225ba.svg)](https://gemnasium.com/9d58ca708c17bb90f829a107e5cb5b03)

[ ![Codeship Status for phpb-com/short-duid](https://codeship.com/projects/f4a57550-05cd-0133-8ce0-3ae97f362442/status?branch=master)](https://codeship.com/projects/89536)

### Changelog
- 1.2.4 - No impact on actual functionality, improved C++ code and updated README with two additional API calls
- 1.2.2 - No impact on actual functionality, added examples and reworked unit tests
- 1.2.0 - A lot of fixes and test additions, also API breaking change: custom_epoch is expecting **milliseconds** instead of seconds
- 1.1.0 - Initial public release

### Requirements
- gcc 4.7+ with C++11 or clang 3.4+
- node.js 0.11+ or iojs 2+
- node-gyp

### Features
- Time and sequence based numeric unique ID generation
- Time and sequence based alphanumeric URL-safe unique ID generation
- Designed to be distributed among 1024 shards, no need to synchronize runtime or after setup
- Can generate 4096 unique IDs per millisecond per shard
- Can generate unique IDs for 139 years without overflow or collision
- Resilient to time drift or sequence overflow, does not delay ID generation
- Allows to set custom epoch, prolong unique ID generation and shorten the ID
- Written in C++11, fast
- No runtime dependencies
- (Convenient Add-on) Encode and decode [hashids](http://hashids.org)
- (Convenient Add-on) Random password generator
- (Convenient Add-on) Random URL-safe API key generator
- Simple to use

### Installation
`npm install node-gyp -g && npm install short-duid`

### How to use
This module is very simple to use, first you will need to initialize it and then start using its instance.

#### API

##### short_duid.init(shard_id, salt, epoch_start)
Instantiates short-duid and sets parameters for the life of instance.

###### Returns
- Short Distributed ID module instance or `Javascript Object` type
    - Initializes new instance of the module with the given parameters

###### Parameters
- `shard_id` - ID of this instance of short-duid, should be unique and not shared with other instances in the cluster; from 0 to 1023. This parameter will be converted into signed 32 bit integer and masked to fit in 12 bits.
- `salt` - Salt that is used by hashid encoder/decoder, should be constant and shared across all nodes in the cluster. Do not change this parameter once used in production, or you will have collisions in the alphanumeric IDs. Good way to generate salt on Linux: `dd if=/dev/random bs=1 count=102400 2>/dev/null| sha256sum`
- `epoch_start` - Number of **milliseconds** since unix epoch (1970, Jan 1 00:00:00 GMT). This should be some date in the near past and should never be changed further into the future once in production. Example: 1433116800000; //Mon, 01 Jun 2015 00:00:00 GMT. This parameter will be converted to unsigned 64bit integer.

____
##### _instance_.getDUID(count)
Method to retrieve array of DUIDs in alphanumeric form. Length of the array is specified by `count` parameter.

###### Returns
- `Javascript array` object of variable length, depending on `count` parameter.
    - Example: `[ "XLz0E3MvkEL" ]`

###### Parameters
- `count` - Number of alphanumeric DUIDs to return, from 0 to 8192.

____
##### _instance_.getDUIDInt(count)
Essential same method as `_instance_.getDUID` but insted of hashid converted integer, will return unique ID in a numeric form as string.

###### Returns
- `Javascript array` object of variable length, depending on `count` parameter.
    - Example: `[ "12534941854212112" ]`

###### Parameters
- `count` - Number of numeric DUIDs to return, from 0 to 8192.

____
##### _instance_.getShardID()
Method to get currently set shard ID of ShortDUID `_instance_`

###### Returns
- `number` current shard ID of ShortDUID `_instance_`
    - Example: `0`

###### Parameters
- `N/A`

____
##### _instance_.getEpochStart()
Method to get currently set custom epoch starting time in milliseconds of ShortDUID `_instance_`

###### Returns
- `string` currently set custome epoch of ShortDUID `_instance_`, since it is unsigned 64bit integer, we return it as string.
    - Example: `"0"`

###### Parameters
- `N/A`

____
##### _instance_.getSalt()
Method to get current salt of ShortDUID `_instance_`. Salt is used to generate alphanumeric DUIDs and also in `hashidEncode`/`hashidDecode` methods.

###### Returns
- `string` currently set salt that is used in hashid conversion of ShortDUID `_instance_`
    - Example: `"this is my salt"`

###### Parameters
- `N/A`

____
##### _instance_.hashidEncode(number_array)
Method to hash(encode) array of unsigned 64bit integers (in `Javascript string` format).

###### Returns
- `string` hashid of array of unsigned 64bit integers
    - Example: `"3nMMYV0PvMl"`

###### Parameters
- `number_array` - Array of unsigned 64bit integers in javascript number or string (if does not fit in `Javascript 58bit integer` data type) form.

____
##### _instance_.hashidDecode(hashid_string)
Decode previously encoded array of numbers with hashid method.

###### Returns
- `Javascript array` array of unsigned 64bit integers in a string form
    - Example: `['1', '2', '3']`

###### Parameters
- `hashid_string` - Hashid in a string form. Example: `3nMMYV0PvMl`

____
##### _instance_.getRandomAPIKey(length)
Method to return randomly generated string of URL-friendly characters that is suitable for use as an API key.

###### Returns
- `string` randomly generated string that is suitable for usage in URL and can serve as good static API key
    - Example: `"JyJ7KqaCBD3nlU6Z0SVafM5MYAXXi29kVdAtaq87PbBFUHnWFBQ0jCdbnOQybNTs"`

###### Parameters
- `length` - Length of the random API key to return, default to 64, can be in the range from 0 to 4096.

____
##### _instance_.getRandomPassword(length)
Method to generate and return string of random characters that are **not** URL-friendly and is mostly suitable as a temporary password.

###### Returns
- `string` randomly generated string that is suitable for usage as a temporary password and is **not** URL-friendly
    - Example: `"*)KTRXa>z^zrSgK8"`

###### Parameters
- `length` - Length of the random password to return, default to 16, can be in the range from 0 to 1024.

____
#### Advanced API
This API is mainly used by unit tests and should not be required for normal usage of the module. Use it at your own risk.

##### _instance_.getCurrentTimeMs()
Method to get current time since unix epoch in milliseconds as seen by the module, not adjusted for custom epoch. This method can be useful in testing and also in capturing _reference_ time to ensure timer stability across restarts.

###### Returns
- `string` of numbers, current time since unix epoch in milliseconds as seen by the module.

###### Parameters
- `N/A`

____
##### _instance_.driftTime(milliseconds)
Method to help simulate `system_clock` drift, can accept positive or negative integers.

###### Returns
- `string` number of milliseconds to drift ShortDUID's internal clock

###### Parameters
- `milliseconds` (optional) number of milliseconds to drif system_clock by, can be a positive or negative integer.


____
#### Example #1
Simplest example to execute all of the major methods of the module.
```javascript
var duid = require('short-duid');
var duid_instance = duid.init(0, "my salt", 0);
console.log(duid_instance.getDUID(10));
console.log(duid_instance.getDUIDInt(10));
console.log(duid_instance.getRandomAPIKey(32));
console.log(duid_instance.getRandomPassword(8));
console.log(duid_instance.hashidEncode([1234]));
console.log(duid_instance.hashidDecode('3nMMYV0PvMl'));
```

#### Example #2
More complete example that will create API server with help of koajs and reply to queries.

##### package.json
```json
{
  "name": "ShortDUIDAPIServer",
  "description": "ShortDUID Example API Service",
  "version": "0.0.1",
  "private": true,
  "dependencies": {
    "koa": "*",
    "koa-bodyparser": "*",
    "koa-response-time": "*",
    "koa-router": "*",
    "koa-json": "*",
    "pm2": "*",
    "short-duid": "*"
  }
}
```

##### index.js
```javascript
'use strict';
var pm2 = require('pm2');

var cpus = require('os').cpus().length;
var procs = Math.ceil(0.75 * cpus);

pm2.connect(function() {
  pm2.start({
    name: 'ShortDUID',
    script: 'api_server.js',
    args: '',
    post_update: ['npm install --save', ' echo Launching ShortDUID'],
    node_args: ['--harmony', '--use_strict'],
    exec_interpreter: 'node',
    next_gen_js: false,
    exec_mode: 'cluster',
    min_uptime: '3600s',
    max_restarts: 25,
    cron_restart: '',
    instances: procs,
    max_memory_restart: '1G',
    error_file: './api_errors.log',
    out_file: './api_info.log',
    pid_file: './short-duid-api.pid',
    merge_logs: true,
    log_date_format: 'YYYY-MM-DD HH:mm:ss Z',
    vizion: true,
    autorestart: true,
    port: 65000,
    env: {
      NODE_ENV: 'production',
      AWS_KEY: 'XXX',
      AWS_SECRET: 'XXX'
    }
  }, function(err, apps) {
    if (err) console.log("Error: ", err, "App:", apps);
    pm2.disconnect();
  });
});
```

##### api_server.js
```javascript
'use strict';
var koa = require('koa');
var router = require('koa-router')();
var app = module.exports = koa();
var duid = require('short-duid');

//Define app name and port for koa-cluster
var cpus = require('os').cpus().length;
app.name = "ShortDUID";
app.node_id = 0;
app.nid = process.env.NODE_APP_INSTANCE ? process.env.NODE_APP_INSTANCE : (process.pid % cpus); //nodejs instance ID
app.shard_id = app.node_id + app.nid;
app.port = 65000;
app.salt = "this is my super secret salt";
app.epoch_start = 1433116800 * 1000; //Mon, 01 Jun 2015 00:00:00 GMT

//Instantiate short-duid
var duid_instance = new duid.init(app.shard_id, app.salt, app.epoch_start);

//Setup routes
router
  .get('/', function*(next) {
    this.body = ({
      name: 'ShortDUID API'
    });
    yield next;
  })
  .get('/random_api_key/:length?', function*(next) {
    this.body = yield([duid_instance.getRandomAPIKey((this.params.length ? this.params.length : 64))]);
    yield next;
  })
  .get('/random_password/:length?', function*(next) {
    this.body = yield([duid_instance.getRandomPassword((this.params.length ? this.params.length : 16))]);
    yield next;
  })
  .get('/hashid_decode/:id', function*(next) {
    this.body = yield(duid_instance.hashidDecode(this.params.id));
    yield next;
  })
  .get('/hashid_encode/:id+', function*(next) {
    this.body = yield([duid_instance.hashidEncode(this.params.id.split('/'))]);
    yield next;
  })
  .get('/nduid/:count?', function*(next) {
    this.body = yield(duid_instance.getDUIDInt((this.params.count ? this.params.count : 1)));
    yield next;
  })
  .get('/duid/:count?', function*(next) {
    this.body = yield(duid_instance.getDUID((this.params.count ? this.params.count : 1)));
    yield next;
  });

//Setup middleware
app
  .use(require('koa-json')())
  .use(require('koa-response-time')())
  .use(router.routes())
  .use(router.allowedMethods());

app.listen(app.port);
```

And then you should install application with `npm install --save` and start the server by `node index.js`. You can check the logs and also list the processes with `./node_modules/.bin/pm2 list`. Getting fresh id can be done by curl: `curl http://localhost:65000/duid/`.

#### More examples
For more examples please see  `examples` folder, which I plan to keep adding to. You are free to contribute more examples.

### Projects using ShortDUID
So far I know of none, if you are using it in your project and do not mind sharing this information, please drop me a note at <ian@phpb.com>, and I will add you to this list.

### Testing
`npm install node-gyp -g && git clone https://gotfix.com/pixnr/short-duid.git && cd short-duid && npm install --save-dev` <br />
`npm test`
```
> ./node_modules/mocha/bin/mocha --reporter spec ./test/



  Short DUID
    #hashidEncode() and #hashidDecode()
      ✓ should produce identical hashids from both instances for: 701098
      ✓ should produce different hashids for two different integers: 701098 and 851606
      ✓ decode should return same integer given output of encode as argument passed to encode: 414993
      ✓ decode should return same array of integers given output of encode as argument passed to encode: 701098,851606,414993
      ✓ should return hashid that is equal to "LeGxr" given [123456] as argument
      ✓ should return hashid that is equal to [ "123456" ] given "LeGxr" as argument
      ✓ should return hashid that is equal to "reG4QhO4NCpm" given [123456,7890,123] as argument
      ✓ should return hashid that is equal to [123456,7890,123] given "reG4QhO4NCpm" as argument
      ✓ should return different hashids given same value and different salt
    #getRandomAPIKey()
      ✓ should return random API key 64 characters long
      ✓ should return random API key each time called, should not be equal
    #getRandomPassword()
      ✓ should return random password 16 characters long
      ✓ should return random password each time called, should not be equal
    #getEpochStart()
      ✓ should return set epoch start, for instance #1: 1433116800000
      ✓ should return set epoch start, for instance #2: 1433116800000
      ✓ instance #1 and instance #2 should return same epoch start: 1433116800000
      ✓ should reset custom epoch to zero if given one larger than real epoch
      ✓ should accept custom epoch that is even 1 millisecond in the past
    #getSalt()
      ✓ should return set salt, for instance #1: 39622feb2b3e7aa7208f50f45ec36fd513baadad6977b53295a3b28aeaed4a54
      ✓ should return set salt, for instance #2: 39622feb2b3e7aa7208f50f45ec36fd513baadad6977b53295a3b28aeaed4a54
      ✓ instance #1 and instance #2 should return same salt: 39622feb2b3e7aa7208f50f45ec36fd513baadad6977b53295a3b28aeaed4a54
    #getShardID()
      ✓ should return set shard id for instance #1: 123
      ✓ should return set shard id for instance #2: 12
      ✓ should return different shard ids for instance #1 and instance #2
    #getDUID()
      ✓ Asked for 1 DUIDs, correctly returns 1 DUIDs
      ✓ Asked for 0 DUIDs, correctly returns 0 DUIDs
      ✓ Asked for 8192 DUIDs, correctly returns 8192 DUIDs
      ✓ Asked for 8193 DUIDs, correctly returns 1 DUIDs
      ✓ should have no duplicates in the returned arrays, 8192 IDs each, and combined. (86ms)
    #getDUIDInt()
      ✓ Asked for 1 Int DUIDs, correctly returns 1 Integer DUIDs
      ✓ Asked for 0 Int DUIDs, correctly returns 0 Integer DUIDs
      ✓ Asked for 8192 Int DUIDs, correctly returns 8192 Integer DUIDs
      ✓ Asked for 8193 Int DUIDs, correctly returns 1 Integer DUIDs
      ✓ should have no duplicates in the returned arrays, 8192 IDs each, and combined. (52ms)
    DUID with drifting time
      ✓ should generate ID with -2881 millisecond drift into the past from now( 1436495534326 ), 14171438882205696 should be numerically smaller than 14171450982772736
      ✓ should consistently generate unique IDs even when time is drifting backwards constantly (160ms)


  36 passing (372ms)
```
## TODO
- Add more tests, time drifting and sequence overflow could be done better than now
- Simplify API further
- Improve error handling, at the moment most of them are silent and prefer overflow
- Add more examples

## Contributing
All are welcome to submit pull requests and patches

## License

The MIT License (MIT)

Copyright (c) 2015 Ian Matyssik <ian@phpb.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
