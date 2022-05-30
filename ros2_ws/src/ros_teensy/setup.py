from setuptools import setup

package_name = 'ros_teensy'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='ineogi2',
    maintainer_email='ineogi2@todo.todo',
    description='For serial tension data : array[6]',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
        	'ros_teensy_publisher = ros_teensy.ros_teensy_publisher:main'
        ],
    },
)
