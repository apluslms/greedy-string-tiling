import setuptools
import setuptools.command.build_ext as build_ext
import subprocess
import codecs
import os

here = os.path.abspath(os.path.dirname(__file__))

with codecs.open(os.path.join(here, 'README.md'), encoding='utf-8') as f:
    readme_file_contents = f.read()


GIT_DEPENDENCIES = (
    ("rollinghashcpp", "https://github.com/lemire/rollinghashcpp.git"),
    # backup fork available at https://github.com/matiaslindgren/rollinghashcpp
)
THIRD_PARTY_DIR = "thirdparty"


class PrepareCommand(setuptools.Command):
    description = "clone dependencies from git remotes"
    user_options = [
        ("output-dir=", "o", "destination directory for all repos"),
    ]

    def initialize_options(self):
        self.output_dir = THIRD_PARTY_DIR

    def finalize_options(self):
        global THIRD_PARTY_DIR
        if self.output_dir is not THIRD_PARTY_DIR:
            THIRD_PARTY_DIR = self.output_dir

    def run(self):
        if not os.path.exists(self.output_dir):
            os.mkdir(self.output_dir)
        for name, url in GIT_DEPENDENCIES:
            out_dir = os.path.join(self.output_dir, name)
            if os.path.exists(out_dir):
                print(out_dir, "already exists, skipping")
                continue
            cmd = ("git", "clone", "--depth", "1" , url, name)
            subprocess.run(cmd, cwd=self.output_dir, check=True)


class PrepareAndBuildCommand(build_ext.build_ext):
    def run(self):
        self.run_command("prepare")
        super().run()


gstmodule = setuptools.Extension(
    'gst',
    sources=[
        # Implementation
        os.path.join('src', 'gst.cpp'),
        # CPython wrapper
        os.path.join('src', 'gstmodule.cpp'),
    ],
    include_dirs=[
        "include",
        os.path.join(THIRD_PARTY_DIR, "rollinghashcpp")
    ],
    extra_compile_args=["--std=c++14"],
)


setuptools.setup(
    name='greedy_string_tiling',
    version='0.3.0',
    description='C++ implementation of the Greedy String Tiling string matching algorithm.',
    long_description=readme_file_contents,
    url='https://github.com/Aalto-LeTech/radar/tree/master/extensions/greedy_string_tiling',

    author='Matias Lindgren',
    author_email='matias.lindgren@gmail.com',
    license='MIT',

    find_packages=True,
    install_requires=[
        'hypothesis', # For testing the built module. Not required for running, but recommended for sanity.
    ],
    cmdclass={
        "prepare": PrepareCommand,
        "build_ext": PrepareAndBuildCommand,
    },
    ext_modules=[
        gstmodule,
    ],
)
