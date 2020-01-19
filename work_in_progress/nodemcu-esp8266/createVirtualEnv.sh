#v!/bin/sh
pyenv="pymcu"
rm -r ${pyenv}
virtualenv --distribute -p python3 ${pyenv}
${pyenv}/bin/pip install -r requirements.txt
echo "run 'source ${pyenv}/bin/activate' to use this environment"
echp "run 'pip freeze > requirements.txt' to save python package versions"
