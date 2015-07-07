# Short Distributed ID generator module

[![Build Status](https://travis-ci.org/phpb-com/short-duid.svg)](https://travis-ci.org/phpb-com/short-duid) [![js-standard-style](https://img.shields.io/badge/code%20style-standard-brightgreen.svg?style=flat-square)](https://github.com/phpb-com/short-duid) [![npm version](https://img.shields.io/npm/v/short-duid.svg?style=flat-square)](https://www.npmjs.com/package/short-duid) [![npm downloads](https://img.shields.io/npm/dm/short-duid.svg?style=flat-square)](https://www.npmjs.com/package/short-duid) [![Code Climate](https://img.shields.io/codeclimate/github/phpb-com/short-duid.svg?style=flat-square)](https://codeclimate.com/github/phpb-com/short-duid) [![Dependency Status](https://david-dm.org/phpb-com/short-duid.svg)](https://david-dm.org/phpb-com/short-duid)

[ ![Codeship Status for phpb-com/short-duid](https://codeship.com/projects/f4a57550-05cd-0133-8ce0-3ae97f362442/status?branch=master)](https://codeship.com/projects/89536)

This module was inspired by [icicle](https://github.com/intenthq/icicle) and [snowflake](https://github.com/twitter/snowflake). The ideas is to be able to generate non-colliding, URL friendly, and relatively short IDs that could be used in any application that requires to create URIs for arbitrary resources.

Looking around for what is available, I failed to find anything that would be simple and easy to implement. As a result, this module was born.

The id is a 64bit unsigned integer with 42bit used for current timestamp in milliseconds, 10 bit used for shard id, and final 12 bit are used for revolving sequence.

| timestamp_ms | shard_id | sequence |
|:---:|:---:|:---:|
| 42bit | 10bit | 12bit |

## short-duid

Table of Contents
=================

  * [Short Distributed ID generator module](#short-distributed-id-generator-module)
    * [short-duid](#short-duid)
      * [Changelog](#changelog)
      * [Requirements](#requirements)
      * [Features](#features)
      * [Installation](#installation)
      * [How to use](#how-to-use)
        * [API](#api)
          * [short_duid.init(shard_id, salt, epoch_start)](#short_duidinitshard_id-salt-epoch_start)
            * [Parameters](#parameters)
          * [(short-duid instance).getDUID(count)](#short-duid-instancegetduidcount)
            * [Parameters](#parameters-1)
          * [(short-duid instance).getDUIDInt(count)](#short-duid-instancegetduidintcount)
            * [Parameters](#parameters-2)
          * [(short-duid instance).getShardID()](#short-duid-instancegetshardid)
            * [Parameters](#parameters-3)
          * [(short-duid instance).getEpochStart()](#short-duid-instancegetepochstart)
            * [Parameters](#parameters-4)
          * [(short-duid instance).getSalt()](#short-duid-instancegetsalt)
            * [Parameters](#parameters-5)
          * [(short-duid instance).hashidEncode(number_array)](#short-duid-instancehashidencodenumber_array)
            * [Parameters](#parameters-6)
          * [(short-duid instance).hashidDecode(hashid_string)](#short-duid-instancehashiddecodehashid_string)
            * [Parameters](#parameters-7)
          * [(short-duid instance).getRandomAPIKey(length)](#short-duid-instancegetrandomapikeylength)
            * [Parameters](#parameters-8)
          * [(short-duid instance).getRandomPassword(length)](#short-duid-instancegetrandompasswordlength)
            * [Parameters](#parameters-9)
        * [Example #1](#example-1)
        * [Example #2](#example-2)
          * [package.json](#packagejson)
          * [index.js](#indexjs)
          * [api_server.js](#api_serverjs)
        * [More examples](#more-examples)
      * [Projects using ShortDUID](#projects-using-shortduid)
      * [Testing](#testing)
    * [TODO](#todo)
    * [Contributing](#contributing)
    * [License](#license)

Created by [gh-md-toc](https://github.com/ekalinin/github-markdown-toc)

### Changelog
- 1.2.2 - No impact on actual functionality, added examples and reworked unit tests
- 1.2.0 - A lot of fixes and test additions, also API breaking change: custom_epoch is expecting **milliseconds** instead of seconds
- 1.1.0 - Initial public release

### Requirements
- gcc 4.7+ with C++11 or clang 3.4+
- node.js 0.11+ or iojs 2.0.0+
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
Instantiates short-duid and sets parameters for the life of instance; returns instance of the module.

###### Parameters
- `shard_id` - ID of this instance of short-duid, should be unique and not shared with other instances in the cluster; from 0 to 1023
- `salt` - Salt that is used by hashid encoder/decoder, should be constant and shared across all nodes in the cluster. Do not change this parameter once used in production, or you will have collisions in the alphanumeric IDs. Good way to generate salt on Linux: `dd if=/dev/random bs=1 count=102400 2>/dev/null| sha256sum`
- `epoch_start` - Number of **milliseconds** since unix epoch (1970, Jan 1 00:00:00 GMT). This should be some date in the near past and should never be changed further into the future once in production. Example: 1433116800000; //Mon, 01 Jun 2015 00:00:00 GMT

<hr />
##### (short-duid instance).getDUID(count)
Returns array of DUIDs in alphanumeric form. Example: `[ "XLz0E3MvkEL" ]`

###### Parameters
- `count` - Number of alphanumeric DUIDs to return, from 0 to 8192.

<hr />
##### (short-duid instance).getDUIDInt(count)
Returns array of DUIDs in numeric form as string. Example: `[ "12534941854212112" ]`

###### Parameters
- `count` - Number of numeric DUIDs to return, from 0 to 8192.

<hr />
##### (short-duid instance).getShardID()
Returns shard ID of the instance. Example: `0`

###### Parameters
- `N/A`

<hr />
##### (short-duid instance).getEpochStart()
Returns custom epoch of the instance. Example: `0`

###### Parameters
- `N/A`

<hr />
##### (short-duid instance).getSalt()
Returns salt of the instance. Example: `"this is my salt"`

###### Parameters
- `N/A`

<hr />
##### (short-duid instance).hashidEncode(number_array)
Returns hashid encoded alphanumeric string, will be dependent on salt. Example: `"3nMMYV0PvMl"`

###### Parameters
- `number_array` - Array of unsigned 64bit integers in javascript number or string form.

<hr />
##### (short-duid instance).hashidDecode(hashid_string)
Returns array of decoded numbers in string form, given hashid as argument; will be dependent on salt. Example: `['1', '2', '3']`

###### Parameters
- `hashid_string` - Hashid in a string form. Example: `3nMMYV0PvMl`

<hr />
##### (short-duid instance).getRandomAPIKey(length)
Returns randomly generated string that is suitable for usage in URL and can serve as good static API key. Default returned length is 64 characters, can generate up to 4096 characters of randomness per call. Example: `"JyJ7KqaCBD3nlU6Z0SVafM5MYAXXi29kVdAtaq87PbBFUHnWFBQ0jCdbnOQybNTs"`

###### Parameters
- `length` - Length of the random API key to return, default to 64, can be up to 4096.

<hr />
##### (short-duid instance).getRandomPassword(length)
Returns random string that is suitable for temporary password, not URL safe. Default returned length is 16 characters, can generate up to 1024 random characters per call. Example: `"*)KTRXa>z^zrSgK8"`

###### Parameters
- `length` - Length of the random password to return, default to 16, can be up to 1024.

<hr />
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
`npm install node-gyp -g && git clone https://github.com/phpb-com/short-duid.git && cd short-duid && npm install --save-dev` <br />
`npm test`
```
2.12s$ npm test
> short-duid@1.2.0 test /home/travis/build/phpb-com/short-duid
> ./node_modules/mocha/bin/mocha --reporter spec ./test/
  Short DUID
    #hashidEncode() and #hashidDecode()
  ✓ should produce identical hashids from both instances for: 530166970
  ✓ should produce different hashids for two different integers: 530166970 and 164008322
  ✓ decode should return same integer given output of encode as argument passed to encode: 92271089
  ✓ decode should return same array of integers given output of encode as argument passed to encode: 530166970,164008322,92271089
  ✓ should return hashid that is equal to "LeGxr" given [123456] as argument
  ✓ should return hashid that is equal to [123456] given "LeGxr" as argument
  ✓ should return hashid that is equal to "reG4QhO4NCpm" given [123456,7890,123] as argument
  ✓ should return hashid that is equal to [123456,7890,123] given "reG4QhO4NCpm" as argument
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
    #getDUID()
  ✓ Asked for 1 DUIDs, correctly returns 1 DUIDs
  ✓ Asked for 8192 DUIDs, correctly returns 8192 DUIDs
  ✓ Asked for 8193 DUIDs, correctly returns 1 DUIDs
  ✓ should have no duplicates in the returned arrays, 8192 IDs each, and combined. (138ms)
    #getDUIDInt()
  ✓ Asked for 1 Int DUIDs, correctly returns 1 Integer DUIDs
  ✓ Asked for 8192 Int DUIDs, correctly returns 8192 Integer DUIDs
  ✓ Asked for 8193 Int DUIDs, correctly returns 1 Integer DUIDs
  ✓ should have no duplicates in the returned arrays, 8192 IDs each, and combined. (109ms)
    DUID with drifting time
  ✓ should generate ID with -6713 millisecond drift into the past from now( 1436230534758 ), 13059950129950720 should be numerically smaller than 13059978307284992
  ✓ should consistently generate unique IDs even when time is drifting backwards constantly (391ms)
  32 passing (715ms)
The command "npm test" exited with 0.
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
