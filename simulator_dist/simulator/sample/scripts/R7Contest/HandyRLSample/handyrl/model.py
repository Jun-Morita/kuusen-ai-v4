# ===== Original Version =====
# Copyright (c) 2020 DeNA Co., Ltd.
# Licensed under The MIT License [see LICENSE for details]
#
# =====Modified Version =====
# Copyright (c) 2021-2025 Air Systems Research Center, Acquisition, Technology & Logistics Agency(ATLA)

# neural nets

import os
os.environ['OMP_NUM_THREADS'] = '1'

import numpy as np
import torch
torch.set_num_threads(1)

import torch.nn as nn
import torch.nn.functional as F

from .util import map_r


def to_torch(x):
    return map_r(x, lambda x: torch.from_numpy(np.array(x)).contiguous() if x is not None else None)


def to_numpy(x):
    return map_r(x, lambda x: x.detach().numpy() if x is not None else None)


def to_gpu(data):
    if torch.cuda.is_available():
        return map_r(data, lambda x: x.cuda() if x is not None else None)
    elif torch.backends.mps.is_available():
        def converter(x):
            if x is None:
                return None
            else:
                if x.dtype == torch.float64:
                    return x.to(torch.float32).to('mps')
                else:
                    return x.to('mps')
        return map_r(data, converter)
    else:
        raise ValueError('No gpu is available.')

# model wrapper class

class ModelWrapper(nn.Module):
    def __init__(self, model):
        super().__init__()
        self.model = model

    @property
    def observation_space(self):
        return self.model.observation_space

    @property
    def action_space(self):
        return self.model.action_space

    @property
    def action_dist_class(self):
        return self.model.action_dist_class

    def get_action_dist(self, params, legal_actions=None, validate_args=None):
        return self.model.get_action_dist(params, legal_actions, validate_args)

    def init_hidden(self, batch_size=None):
        if hasattr(self.model, 'init_hidden'):
            if batch_size is None:  # for inference
                hidden = self.model.init_hidden([])
                return map_r(hidden, lambda h: h.detach().numpy() if isinstance(h, torch.Tensor) else h)
            else:  # for training
                return self.model.init_hidden(batch_size)
        return None

    def forward(self, *args, **kwargs):
        return self.model.forward(*args, **kwargs)

    def inference(self, x, hidden, **kwargs):
        # numpy array -> numpy array
        if hasattr(self.model, 'inference'):
            return self.model.inference(x, hidden, **kwargs)

        self.eval()
        with torch.no_grad():
            xt = map_r(x, lambda x: torch.from_numpy(np.array(x)).contiguous().unsqueeze(0) if x is not None else None)
            ht = map_r(hidden, lambda h: torch.from_numpy(np.array(h)).contiguous().unsqueeze(0) if h is not None else None)
            outputs = self.forward(xt, ht, **kwargs)
        return map_r(outputs, lambda o: o.detach().numpy().squeeze(0) if o is not None else None)
