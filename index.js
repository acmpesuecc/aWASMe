// a lil bit of boiler plate code
var factory = require('./engine.js');

globalThis.exported_fns = {
	0: (arg1) => {
		console.log(arg1);
		return 69;
	}
};

factory().then((instance) => {
	globalThis.Module = instance;	
	instance._do_stuff(); 
});


