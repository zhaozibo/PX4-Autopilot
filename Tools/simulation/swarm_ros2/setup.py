from setuptools import setup
import os
from glob import glob

package_name = 'swarm_ros2'

setup(
    name=package_name,
    version='0.1.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages', ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'), glob('launch/*.py')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Ramon Roche',
    maintainer_email='mrpollo@gmail.com',
    description='Swarm coordination for PX4 multi-vehicle simulation',
    license='BSD-3-Clause',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'vision_provider = swarm_ros2.vision_provider:main',
            'swarm_controller = swarm_ros2.swarm_controller:main',
        ],
    },
)
