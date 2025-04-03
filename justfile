init-uv:
    #!/opt/homebrew/bin/fish
    uv venv
    source .venv/bin/activate.fish
    uv add --dev ipykernel

reqs:
    uv pip install -r requirements.txt