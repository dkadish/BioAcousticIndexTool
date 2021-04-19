import os
from pathlib import Path
import csv
import itertools
import subprocess


def main(directory):
    d = Path(directory)
    i = itertools.count()
    old_names = filter(lambda f: f[-3:] == 'wav', os.listdir(d))
    with open('filenames.csv', 'w', newline='') as csvfile:
        fieldnames = ['original','short']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for f in old_names:
            n = next(i)
            new_name = '{}.wav'.format(n)
            # (d / f).rename(new_name)
            subprocess.run(['ffmpeg','-i', d / f,'-ar','44100', d / new_name])
            writer.writerow({'original': f, 'short': new_name})

if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser(description='Reformat and rename audio files.')
    parser.add_argument('directory', metavar='D', help='Directory with audio files.')

    args = parser.parse_args()
    main(**dict(args._get_kwargs()))
