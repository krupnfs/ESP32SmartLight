# Patch AsyncElegantOTA to remove #error deprecation warning
Import('env')

Import('env')

def patch_async_elegant_ota(source, target, env):
    import os
    lib_dir = os.path.join('.pio', 'libdeps', env.get('PIOENV'), 'AsyncElegantOTA', 'src')
    header_file = os.path.join(lib_dir, 'AsyncElegantOTA.h')
    if os.path.exists(header_file):
        with open(header_file, 'r') as f:
            content = f.read()
        if '#error AsyncElegantOTA' in content:
            content = content.replace(
                '#error AsyncElegantOTA library is deprecated, Please consider migrating to newer ElegantOTA library which now comes with an async mode. Learn More: https://docs.elegantota.pro/async-mode/ ',
                '// Deprecated warning disabled\n//#error AsyncElegantOTA library is deprecated'
            )
            with open(header_file, 'w') as f:
                f.write(content)
            print("Patched AsyncElegantOTA.h - disabled #error deprecation warning")

env.AddPreAction('buildprog', patch_async_elegant_ota)
