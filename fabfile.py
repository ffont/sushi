import os

from fabric import task, Connection

host = 'mind@elk-pi.local'
remote_dir = '/home/mind/'


@task
def compile_elk(ctx):

    # Cross-compile Source
    print('Coss-compiling Sushi for ELK platform...')
    print('****************************************\n')
    os.system('docker run --rm -it -v elkvolume:/workdir -v ${PWD}/:/code/sushi -v ${PWD}/../VST_SDK/VST2_SDK:/code/VST2_SDK -v ${PWD}/custom-esdk-launch.py:/usr/bin/esdk-launch.py crops/extsdk-container')

    print('\nAll done!')


@task
def send_elk(ctx):

    # Send buils sushi to elk board
    print('\nSending compiled sushi to board...')
    print('**********************************\n')
    with Connection(host=host, connect_kwargs={'password': 'elk'}) as c:
        for local_file, destination_dir in [
            ("build/release/sushi", remote_dir + 'sushi_custom')
        ]:
            print('- Copying {0} to {1}'.format(local_file, destination_dir))
            c.put(local_file, destination_dir)

    print('\nAll done!')
    print('\n')